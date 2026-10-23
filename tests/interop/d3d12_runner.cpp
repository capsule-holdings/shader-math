// D3D12 side of the DXIL cross-check: runs a compute kernel compiled by DXC
// to DXIL once on WARP (the software D3D12 device that ships with Windows)
// and dumps "name[i] 0xBITS" lines, like vk_runner. The case names come from
// the X-macro named by SM_D3D12_CASES in SM_D3D12_CASES_HEADER (set per
// target by CMake); the expressions are never compiled here.
//
//   <runner> <kernel.dxil> [--f16]
//
// --f16 needs Native16BitShaderOpsSupported; WARP without it exits 77
// (ctest skip).
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#include SM_D3D12_CASES_HEADER

using Microsoft::WRL::ComPtr;

namespace {

const char* const kNames[] = {
#define X(name, expr) #name,
    SM_D3D12_CASES(X)
#undef X
};
constexpr int kCases = static_cast<int>(sizeof(kNames) / sizeof(kNames[0]));

bool ok(HRESULT hr, const char* what) {
    if (FAILED(hr)) std::fprintf(stderr, "%s failed: 0x%08lx\n", what, static_cast<unsigned long>(hr));
    return SUCCEEDED(hr);
}

D3D12_RESOURCE_DESC buffer_desc(UINT64 size, D3D12_RESOURCE_FLAGS flags) {
    D3D12_RESOURCE_DESC d{};
    d.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    d.Width = size;
    d.Height = 1;
    d.DepthOrArraySize = 1;
    d.MipLevels = 1;
    d.Format = DXGI_FORMAT_UNKNOWN;
    d.SampleDesc.Count = 1;
    d.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    d.Flags = flags;
    return d;
}

D3D12_RESOURCE_BARRIER transition(ID3D12Resource* r, D3D12_RESOURCE_STATES from,
                                  D3D12_RESOURCE_STATES to) {
    D3D12_RESOURCE_BARRIER b{};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b.Transition.pResource = r;
    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b.Transition.StateBefore = from;
    b.Transition.StateAfter = to;
    return b;
}

}  // namespace

int main(int argc, char** argv) {
    const bool want_f16 = argc == 3 && std::strcmp(argv[2], "--f16") == 0;
    if (argc != 2 && !want_f16) {
        std::fprintf(stderr, "usage: %s <kernel.dxil> [--f16]\n", argv[0]);
        return 1;
    }
    std::ifstream f(argv[1], std::ios::binary);
    const std::vector<char> dxil((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (dxil.empty()) {
        std::fprintf(stderr, "cannot read DXIL from %s\n", argv[1]);
        return 1;
    }

    ComPtr<IDXGIFactory4> factory;
    if (!ok(CreateDXGIFactory1(IID_PPV_ARGS(&factory)), "CreateDXGIFactory1")) return 1;
    ComPtr<IDXGIAdapter> adapter;
    if (!ok(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "EnumWarpAdapter")) return 1;
    DXGI_ADAPTER_DESC ad{};
    adapter->GetDesc(&ad);
    std::fprintf(stderr, "device: %ls\n", ad.Description);
    ComPtr<ID3D12Device> dev;
    if (!ok(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dev)),
            "D3D12CreateDevice"))
        return 1;

    // the kernels are cs_6_2
    D3D12_FEATURE_DATA_SHADER_MODEL sm{D3D_SHADER_MODEL_6_2};
    if (FAILED(dev->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &sm, sizeof(sm))) ||
        sm.HighestShaderModel < D3D_SHADER_MODEL_6_2) {
        std::fprintf(stderr, "device has no shader model 6.2\n");
        return 1;
    }
    if (want_f16) {
        D3D12_FEATURE_DATA_D3D12_OPTIONS4 o4{};
        if (FAILED(dev->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &o4, sizeof(o4))) ||
            !o4.Native16BitShaderOpsSupported) {
            std::fprintf(stderr, "SKIP: %ls has no native 16-bit shader ops\n", ad.Description);
            return 77;
        }
    }

    // one root UAV at u0 (the kernel's RWStructuredBuffer<float>)
    D3D12_ROOT_PARAMETER param{};
    param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
    param.Descriptor.ShaderRegister = 0;
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    D3D12_ROOT_SIGNATURE_DESC rsd{};
    rsd.NumParameters = 1;
    rsd.pParameters = &param;
    ComPtr<ID3DBlob> rs_blob, rs_err;
    if (FAILED(D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &rs_blob, &rs_err))) {
        std::fprintf(stderr, "D3D12SerializeRootSignature failed: %s\n",
                     rs_err ? static_cast<const char*>(rs_err->GetBufferPointer()) : "");
        return 1;
    }
    ComPtr<ID3D12RootSignature> rs;
    if (!ok(dev->CreateRootSignature(0, rs_blob->GetBufferPointer(), rs_blob->GetBufferSize(),
                                     IID_PPV_ARGS(&rs)),
            "CreateRootSignature"))
        return 1;

    D3D12_COMPUTE_PIPELINE_STATE_DESC pd{};
    pd.pRootSignature = rs.Get();
    pd.CS.pShaderBytecode = dxil.data();
    pd.CS.BytecodeLength = dxil.size();
    ComPtr<ID3D12PipelineState> pso;
    if (!ok(dev->CreateComputePipelineState(&pd, IID_PPV_ARGS(&pso)), "CreateComputePipelineState"))
        return 1;

    const UINT64 size = sizeof(float) * kCases * 4;
    D3D12_HEAP_PROPERTIES default_heap{};
    default_heap.Type = D3D12_HEAP_TYPE_DEFAULT;
    D3D12_HEAP_PROPERTIES readback_heap{};
    readback_heap.Type = D3D12_HEAP_TYPE_READBACK;
    const D3D12_RESOURCE_DESC uav_desc =
        buffer_desc(size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    const D3D12_RESOURCE_DESC rb_desc = buffer_desc(size, D3D12_RESOURCE_FLAG_NONE);
    ComPtr<ID3D12Resource> buf, readback;
    if (!ok(dev->CreateCommittedResource(&default_heap, D3D12_HEAP_FLAG_NONE, &uav_desc,
                                         D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buf)),
            "CreateCommittedResource (uav)") ||
        !ok(dev->CreateCommittedResource(&readback_heap, D3D12_HEAP_FLAG_NONE, &rb_desc,
                                         D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                         IID_PPV_ARGS(&readback)),
            "CreateCommittedResource (readback)"))
        return 1;

    D3D12_COMMAND_QUEUE_DESC qd{};
    qd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ComPtr<ID3D12CommandQueue> queue;
    ComPtr<ID3D12CommandAllocator> alloc;
    ComPtr<ID3D12GraphicsCommandList> cl;
    if (!ok(dev->CreateCommandQueue(&qd, IID_PPV_ARGS(&queue)), "CreateCommandQueue") ||
        !ok(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc)),
            "CreateCommandAllocator") ||
        !ok(dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc.Get(), pso.Get(),
                                   IID_PPV_ARGS(&cl)),
            "CreateCommandList"))
        return 1;

    const D3D12_RESOURCE_BARRIER to_uav =
        transition(buf.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    const D3D12_RESOURCE_BARRIER to_copy = transition(
        buf.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
    cl->ResourceBarrier(1, &to_uav);
    cl->SetComputeRootSignature(rs.Get());
    cl->SetComputeRootUnorderedAccessView(0, buf->GetGPUVirtualAddress());
    cl->Dispatch(1, 1, 1);
    cl->ResourceBarrier(1, &to_copy);
    cl->CopyResource(readback.Get(), buf.Get());
    if (!ok(cl->Close(), "Close")) return 1;
    ID3D12CommandList* lists[] = {cl.Get()};
    queue->ExecuteCommandLists(1, lists);

    ComPtr<ID3D12Fence> fence;
    if (!ok(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "CreateFence"))
        return 1;
    const HANDLE done = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!done || !ok(queue->Signal(fence.Get(), 1), "Signal") ||
        !ok(fence->SetEventOnCompletion(1, done), "SetEventOnCompletion"))
        return 1;
    WaitForSingleObject(done, INFINITE);
    CloseHandle(done);
    if (!ok(dev->GetDeviceRemovedReason(), "device removed")) return 1;

    void* mapped = nullptr;
    const D3D12_RANGE read{0, static_cast<SIZE_T>(size)};
    if (!ok(readback->Map(0, &read, &mapped), "Map")) return 1;
    std::vector<float> out(kCases * 4);
    std::memcpy(out.data(), mapped, static_cast<size_t>(size));
    const D3D12_RANGE written{0, 0};
    readback->Unmap(0, &written);

    for (int c = 0; c < kCases; ++c) {
        for (int i = 0; i < 4; ++i) {
            std::uint32_t bits;
            std::memcpy(&bits, &out[c * 4 + i], sizeof(bits));
            std::printf("%s[%d] 0x%08x\n", kNames[c], i, bits);
        }
    }
    return 0;
}
