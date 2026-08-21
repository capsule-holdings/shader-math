// Minimal skinned-mesh demo shaders (Slang -> SPIR-V, see build_shaders.sh).
// Skinning itself happens on the CPU with shader-math; the GPU side only
// applies the view-projection transform and a simple N.L shade.
//
// SDL_GPU SPIR-V resource convention: vertex uniform buffers live in
// descriptor set 1, fragment uniform buffers in set 3.

[[vk::binding(0, 1)]]
cbuffer VSParams
{
    row_major float4x4 viewProj;  // rows match shader-math hlsl::float4x4 memory
}

struct VSIn
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
};

struct VSOut
{
    float4 pos : SV_Position;
    float3 nrm : NORMAL;
};

[shader("vertex")]
VSOut vsMain(VSIn i)
{
    VSOut o;
    o.pos = mul(viewProj, float4(i.pos, 1.0f));
    o.nrm = i.nrm;
    return o;
}

[shader("fragment")]
float4 fsMain(VSOut i) : SV_Target
{
    float3 l = normalize(float3(0.4f, 0.8f, 0.45f));
    float3 n = normalize(i.nrm);
    float ndl = max(dot(n, l), 0.0f) * 0.85f + 0.15f;
    return float4(ndl * 0.9f, ndl * 0.75f, ndl * 0.4f, 1.0f);
}
