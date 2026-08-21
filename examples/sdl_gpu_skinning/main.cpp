// SDL_GPU + ufbx + shader-math skinned-mesh demo.
//
//   FBX (ufbx) -> CPU linear-blend skinning with shader-math (hlsl surface,
//   SIMD mul/normalize) -> SDL_GPU (Vulkan/D3D12/Metal) flat-ish shading or
//   wireframe.
//
// Modes:
//   default            windowed, animated
//   --headless N       render N frames offscreen and write frame_XX.bmp
//                      (works on software Vulkan / lavapipe, no display)
//   --wireframe        line rasterization instead of fill
//   --fbx PATH         load a different FBX (default: tentacle.fbx)
//   --out DIR          output directory for headless frames (default: .)
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <shader_math/shader_math.hpp>
#include <ufbx.h>

using namespace shader_math::hlsl;

// ---- small camera helpers on shader-math types (column-vector convention) --
static float4x4 perspective_vk(float fovy, float aspect, float zn, float zf) {
    const float f = 1.0f / std::tan(fovy * 0.5f);
    // D3D/Vulkan-style z in [0,1]; y negated for Vulkan's downward clip y.
    return float4x4(f / aspect, 0.0f, 0.0f, 0.0f,
                    0.0f, -f, 0.0f, 0.0f,
                    0.0f, 0.0f, zf / (zn - zf), zn * zf / (zn - zf),
                    0.0f, 0.0f, -1.0f, 0.0f);
}

static float4x4 look_at(float3 eye, float3 target, float3 up) {
    const float3 zax = normalize(eye - target);
    const float3 xax = normalize(cross(up, zax));
    const float3 yax = cross(zax, xax);
    return float4x4(xax[0], xax[1], xax[2], -dot(xax, eye),
                    yax[0], yax[1], yax[2], -dot(yax, eye),
                    zax[0], zax[1], zax[2], -dot(zax, eye),
                    0.0f, 0.0f, 0.0f, 1.0f);
}

static float4x4 from_ufbx(const ufbx_matrix& m) {
    // ufbx matrices are 3x4 column arrays; shader-math fills row-major.
    return float4x4(
        (float)m.cols[0].x, (float)m.cols[1].x, (float)m.cols[2].x, (float)m.cols[3].x,
        (float)m.cols[0].y, (float)m.cols[1].y, (float)m.cols[2].y, (float)m.cols[3].y,
        (float)m.cols[0].z, (float)m.cols[1].z, (float)m.cols[2].z, (float)m.cols[3].z,
        0.0f, 0.0f, 0.0f, 1.0f);
}

// ---- skinned mesh data ------------------------------------------------------
struct Corner {           // one triangle corner (expanded, no index reuse)
    float3 rest_pos;
    float3 rest_nrm;
    int joint[4];
    float weight[4];
};

struct GpuVertex {        // what the GPU sees (pitch 24)
    float px, py, pz;
    float nx, ny, nz;
};

struct SkinnedMesh {
    std::vector<Corner> corners;          // 3 per triangle
    std::vector<ufbx_node*> joints;       // cluster -> bone node
    std::vector<float4x4> geom_to_bone;   // cluster bind matrices
};

static bool build_mesh(ufbx_scene* scene, SkinnedMesh& out) {
    ufbx_mesh* mesh = nullptr;
    ufbx_skin_deformer* skin = nullptr;
    for (size_t i = 0; i < scene->meshes.count && !mesh; ++i) {
        if (scene->meshes.data[i]->skin_deformers.count > 0) {
            mesh = scene->meshes.data[i];
            skin = mesh->skin_deformers.data[0];
        }
    }
    if (!mesh) {
        std::fprintf(stderr, "no skinned mesh found in FBX\n");
        return false;
    }

    out.joints.reserve(skin->clusters.count);
    for (size_t c = 0; c < skin->clusters.count; ++c) {
        ufbx_skin_cluster* cluster = skin->clusters.data[c];
        out.joints.push_back(cluster->bone_node);
        out.geom_to_bone.push_back(from_ufbx(cluster->geometry_to_bone));
    }

    std::vector<uint32_t> tri(mesh->max_face_triangles * 3);
    for (size_t f = 0; f < mesh->faces.count; ++f) {
        const size_t ntri =
            ufbx_triangulate_face(tri.data(), tri.size(), mesh, mesh->faces.data[f]);
        for (size_t t = 0; t < ntri * 3; ++t) {
            const uint32_t ix = tri[t];
            Corner corner{};
            const ufbx_vec3 p = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
            const ufbx_vec3 n = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
            corner.rest_pos = float3((float)p.x, (float)p.y, (float)p.z);
            corner.rest_nrm = float3((float)n.x, (float)n.y, (float)n.z);

            const uint32_t cp = mesh->vertex_indices.data[ix];
            const ufbx_skin_vertex sv = skin->vertices.data[cp];
            float total = 0.0f;
            for (uint32_t w = 0; w < sv.num_weights && w < 4; ++w) {
                const ufbx_skin_weight sw = skin->weights.data[sv.weight_begin + w];
                corner.joint[w] = (int)sw.cluster_index;
                corner.weight[w] = (float)sw.weight;
                total += corner.weight[w];
            }
            if (total > 0.0f)
                for (int w = 0; w < 4; ++w) corner.weight[w] /= total;
            out.corners.push_back(corner);
        }
    }
    std::printf("mesh: %zu triangles, %zu joints\n", out.corners.size() / 3,
                out.joints.size());
    return true;
}

// ---- the shader-math part: CPU linear-blend skinning ------------------------
// Per joint: palette = node_to_world(t) * geometry_to_bone  (float4x4 mul)
// Per corner: pos' = sum_j w_j * mul(palette_j, float4(pos, 1))
static void skin_mesh(const SkinnedMesh& mesh, const std::vector<float4x4>& palette,
                      std::vector<GpuVertex>& out) {
    out.resize(mesh.corners.size());
    for (size_t i = 0; i < mesh.corners.size(); ++i) {
        const Corner& c = mesh.corners[i];
        const float4 rp(c.rest_pos, 1.0f);
        const float4 rn(c.rest_nrm, 0.0f);
        float4 p = float4(0.0f);
        float4 n = float4(0.0f);
        for (int w = 0; w < 4; ++w) {
            if (c.weight[w] <= 0.0f) continue;
            const float4x4& m = palette[c.joint[w]];
            p += mul(m, rp) * c.weight[w];
            n += mul(m, rn) * c.weight[w];
        }
        const float3 nn = normalize(float3(n[0], n[1], n[2]));
        out[i] = {p[0], p[1], p[2], nn[0], nn[1], nn[2]};
    }
}

// ---- SDL_GPU plumbing --------------------------------------------------------
static std::vector<uint8_t> read_file(const std::string& path) {
    std::vector<uint8_t> data;
    if (SDL_IOStream* io = SDL_IOFromFile(path.c_str(), "rb")) {
        const Sint64 size = SDL_GetIOSize(io);
        if (size > 0) {
            data.resize((size_t)size);
            SDL_ReadIO(io, data.data(), (size_t)size);
        }
        SDL_CloseIO(io);
    }
    return data;
}

static SDL_GPUShader* load_shader(SDL_GPUDevice* dev, const std::string& path,
                                  SDL_GPUShaderStage stage, Uint32 uniform_buffers) {
    const std::vector<uint8_t> code = read_file(path);
    if (code.empty()) {
        std::fprintf(stderr, "cannot read shader %s\n", path.c_str());
        return nullptr;
    }
    SDL_GPUShaderCreateInfo info{};
    info.code_size = code.size();
    info.code = code.data();
    info.entrypoint = "main";  // slangc renames the SPIR-V entry point to main
    info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    info.stage = stage;
    info.num_uniform_buffers = uniform_buffers;
    return SDL_CreateGPUShader(dev, &info);
}

int main(int argc, char** argv) {
    int headless_frames = 0;
    bool wireframe = false;
    std::string asset_dir = SM_DEMO_ASSET_DIR;
    std::string fbx_path = asset_dir + "/tentacle.fbx";
    std::string out_dir = ".";
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "--headless" && i + 1 < argc) headless_frames = std::atoi(argv[++i]);
        else if (a == "--wireframe") wireframe = true;
        else if (a == "--fbx" && i + 1 < argc) fbx_path = argv[++i];
        else if (a == "--out" && i + 1 < argc) out_dir = argv[++i];
    }
    const bool headless = headless_frames > 0;
    if (headless) SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "offscreen");

    // ---- FBX ----------------------------------------------------------------
    ufbx_load_opts opts{};
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.target_unit_meters = 1.0f;
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(fbx_path.c_str(), &opts, &error);
    if (!scene) {
        std::fprintf(stderr, "ufbx: %s (%s)\n", error.description.data, fbx_path.c_str());
        return 1;
    }
    SkinnedMesh mesh;
    if (!build_mesh(scene, mesh)) return 1;
    ufbx_anim* anim = scene->anim;
    const double anim_len =
        anim && anim->time_end > anim->time_begin ? anim->time_end - anim->time_begin : 2.0;

    // ---- SDL / GPU ------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GPUDevice* dev = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!dev) {
        std::fprintf(stderr, "SDL_CreateGPUDevice: %s\n", SDL_GetError());
        return 1;
    }
    std::printf("GPU driver: %s\n", SDL_GetGPUDeviceDriver(dev));

    const Uint32 width = 640;
    const Uint32 height = 480;
    SDL_Window* window = nullptr;
    SDL_GPUTextureFormat color_format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    if (!headless) {
        window = SDL_CreateWindow("shader-math SDL_GPU skinning", (int)width, (int)height,
                                  SDL_WINDOW_RESIZABLE);
        if (!window || !SDL_ClaimWindowForGPUDevice(dev, window)) {
            std::fprintf(stderr, "window/claim: %s\n", SDL_GetError());
            return 1;
        }
        color_format = SDL_GetGPUSwapchainTextureFormat(dev, window);
    }

    // ---- shaders / pipeline ----------------------------------------------------
    SDL_GPUShader* vs =
        load_shader(dev, asset_dir + "/shader.vert.spv", SDL_GPU_SHADERSTAGE_VERTEX, 1);
    SDL_GPUShader* fs =
        load_shader(dev, asset_dir + "/shader.frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 0);
    if (!vs || !fs) {
        std::fprintf(stderr, "shader load failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GPUVertexBufferDescription vbd{};
    vbd.slot = 0;
    vbd.pitch = sizeof(GpuVertex);
    vbd.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    SDL_GPUVertexAttribute attrs[2]{};
    attrs[0].location = 0;
    attrs[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attrs[0].offset = 0;
    attrs[1].location = 1;
    attrs[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    attrs[1].offset = 12;

    SDL_GPUColorTargetDescription ctd{};
    ctd.format = color_format;

    SDL_GPUGraphicsPipelineCreateInfo pi{};
    pi.vertex_shader = vs;
    pi.fragment_shader = fs;
    pi.vertex_input_state.vertex_buffer_descriptions = &vbd;
    pi.vertex_input_state.num_vertex_buffers = 1;
    pi.vertex_input_state.vertex_attributes = attrs;
    pi.vertex_input_state.num_vertex_attributes = 2;
    pi.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pi.rasterizer_state.fill_mode =
        wireframe ? SDL_GPU_FILLMODE_LINE : SDL_GPU_FILLMODE_FILL;
    pi.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pi.depth_stencil_state.enable_depth_test = true;
    pi.depth_stencil_state.enable_depth_write = true;
    pi.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
    pi.target_info.color_target_descriptions = &ctd;
    pi.target_info.num_color_targets = 1;
    pi.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
    pi.target_info.has_depth_stencil_target = true;
    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(dev, &pi);
    if (!pipeline) {
        std::fprintf(stderr, "pipeline: %s\n", SDL_GetError());
        return 1;
    }

    // ---- GPU resources -----------------------------------------------------------
    const Uint32 vbuf_size = (Uint32)(mesh.corners.size() * sizeof(GpuVertex));
    SDL_GPUBufferCreateInfo bci{};
    bci.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bci.size = vbuf_size;
    SDL_GPUBuffer* vbuf = SDL_CreateGPUBuffer(dev, &bci);

    SDL_GPUTransferBufferCreateInfo tci{};
    tci.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tci.size = vbuf_size;
    SDL_GPUTransferBuffer* upload = SDL_CreateGPUTransferBuffer(dev, &tci);

    SDL_GPUTexture* depth = nullptr;
    SDL_GPUTexture* offscreen = nullptr;
    SDL_GPUTransferBuffer* readback = nullptr;
    {
        SDL_GPUTextureCreateInfo di{};
        di.type = SDL_GPU_TEXTURETYPE_2D;
        di.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
        di.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
        di.width = width;
        di.height = height;
        di.layer_count_or_depth = 1;
        di.num_levels = 1;
        depth = SDL_CreateGPUTexture(dev, &di);
    }
    if (headless) {
        SDL_GPUTextureCreateInfo ci{};
        ci.type = SDL_GPU_TEXTURETYPE_2D;
        ci.format = color_format;
        ci.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
        ci.width = width;
        ci.height = height;
        ci.layer_count_or_depth = 1;
        ci.num_levels = 1;
        offscreen = SDL_CreateGPUTexture(dev, &ci);
        SDL_GPUTransferBufferCreateInfo ri{};
        ri.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
        ri.size = width * height * 4;
        readback = SDL_CreateGPUTransferBuffer(dev, &ri);
    }

    // ---- frame loop -----------------------------------------------------------
    std::vector<float4x4> palette(mesh.joints.size());
    std::vector<GpuVertex> vertices;
    const float4x4 proj = perspective_vk(1.0f, (float)width / (float)height, 0.05f, 50.0f);
    const float4x4 view = look_at(float3(2.4f, 1.9f, 2.4f), float3(0.0f, 0.9f, 0.0f),
                                  float3(0.0f, 1.0f, 0.0f));
    const float4x4 view_proj = mul(proj, view);  // shader-math matrix product

    const int total_frames = headless ? headless_frames : -1;
    bool running = true;
    int frame = 0;
    const Uint64 t0 = SDL_GetTicks();
    while (running && (total_frames < 0 || frame < total_frames)) {
        if (!headless) {
            SDL_Event ev;
            while (SDL_PollEvent(&ev))
                if (ev.type == SDL_EVENT_QUIT) running = false;
        }
        const double t =
            headless ? (double)frame / 24.0 : (double)(SDL_GetTicks() - t0) / 1000.0;
        const double anim_t =
            (anim ? anim->time_begin : 0.0) + std::fmod(t, anim_len);

        // evaluate the animation, build the joint palette, skin on the CPU
        ufbx_scene* eval = ufbx_evaluate_scene(scene, anim, anim_t, nullptr, nullptr);
        for (size_t j = 0; j < mesh.joints.size(); ++j) {
            const ufbx_node* node = eval->nodes.data[mesh.joints[j]->typed_id];
            palette[j] = mul(from_ufbx(node->node_to_world), mesh.geom_to_bone[j]);
        }
        skin_mesh(mesh, palette, vertices);
        ufbx_free_scene(eval);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(dev);

        // upload skinned vertices
        void* mapped = SDL_MapGPUTransferBuffer(dev, upload, true);
        std::memcpy(mapped, vertices.data(), vbuf_size);
        SDL_UnmapGPUTransferBuffer(dev, upload);
        {
            SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
            SDL_GPUTransferBufferLocation src{upload, 0};
            SDL_GPUBufferRegion dst{vbuf, 0, vbuf_size};
            SDL_UploadToGPUBuffer(copy, &src, &dst, true);
            SDL_EndGPUCopyPass(copy);
        }

        SDL_GPUTexture* target = offscreen;
        if (!headless) {
            if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &target, nullptr,
                                                       nullptr) ||
                !target) {
                SDL_SubmitGPUCommandBuffer(cmd);
                continue;
            }
        }

        SDL_GPUColorTargetInfo cti{};
        cti.texture = target;
        cti.clear_color = SDL_FColor{0.08f, 0.09f, 0.11f, 1.0f};
        cti.load_op = SDL_GPU_LOADOP_CLEAR;
        cti.store_op = SDL_GPU_STOREOP_STORE;
        SDL_GPUDepthStencilTargetInfo dsti{};
        dsti.texture = depth;
        dsti.clear_depth = 1.0f;
        dsti.load_op = SDL_GPU_LOADOP_CLEAR;
        dsti.store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_GPURenderPass* pass = SDL_BeginGPURenderPass(cmd, &cti, 1, &dsti);
        SDL_BindGPUGraphicsPipeline(pass, pipeline);
        SDL_GPUBufferBinding vb{vbuf, 0};
        SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
        SDL_PushGPUVertexUniformData(cmd, 0, &view_proj, sizeof(view_proj));
        SDL_DrawGPUPrimitives(pass, (Uint32)vertices.size(), 1, 0, 0);
        SDL_EndGPURenderPass(pass);

        if (headless) {
            SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
            SDL_GPUTextureRegion srcr{};
            srcr.texture = target;
            srcr.w = width;
            srcr.h = height;
            srcr.d = 1;
            SDL_GPUTextureTransferInfo dsti2{};
            dsti2.transfer_buffer = readback;
            SDL_DownloadFromGPUTexture(copy, &srcr, &dsti2);
            SDL_EndGPUCopyPass(copy);

            SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
            SDL_WaitForGPUFences(dev, true, &fence, 1);
            SDL_ReleaseGPUFence(dev, fence);

            void* pixels = SDL_MapGPUTransferBuffer(dev, readback, false);
            SDL_Surface* surf = SDL_CreateSurfaceFrom(
                (int)width, (int)height, SDL_PIXELFORMAT_ABGR8888, pixels,
                (int)(width * 4));
            char name[512];
            std::snprintf(name, sizeof(name), "%s/frame_%02d.bmp", out_dir.c_str(),
                          frame);
            SDL_SaveBMP(surf, name);
            SDL_DestroySurface(surf);
            SDL_UnmapGPUTransferBuffer(dev, readback);
            std::printf("wrote %s\n", name);
        } else {
            SDL_SubmitGPUCommandBuffer(cmd);
        }
        ++frame;
    }

    ufbx_free_scene(scene);
    SDL_DestroyGPUDevice(dev);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
