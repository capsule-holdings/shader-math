// Vulkan side of the GLSL and half cross-checks: runs a compute kernel
// (GLSL via glslang, or Slang via -target spirv) once on the first Vulkan
// device (lavapipe in CI) and dumps "name[i] 0xBITS" lines. The case names
// come from the X-macro named by SM_VK_CASES in SM_VK_CASES_HEADER (set per
// target by CMake); the expressions are never compiled here.
//
//   <runner> <kernel.spv> [--f16]
//
// --f16 enables shaderFloat16; a device without it exits 77 (ctest skip).
#include <vulkan/vulkan.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

#include SM_VK_CASES_HEADER

namespace {

const char* const kNames[] = {
#define X(name, expr) #name,
    SM_VK_CASES(X)
#undef X
};
constexpr int kCases = static_cast<int>(sizeof(kNames) / sizeof(kNames[0]));

bool ok(VkResult r, const char* what) {
    if (r != VK_SUCCESS) std::fprintf(stderr, "%s failed: %d\n", what, static_cast<int>(r));
    return r == VK_SUCCESS;
}

}  // namespace

int main(int argc, char** argv) {
    const bool want_f16 = argc == 3 && std::strcmp(argv[2], "--f16") == 0;
    if (argc != 2 && !want_f16) {
        std::fprintf(stderr, "usage: %s <kernel.spv> [--f16]\n", argv[0]);
        return 1;
    }
    std::ifstream f(argv[1], std::ios::binary);
    const std::vector<char> spv((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (spv.empty() || spv.size() % 4 != 0) {
        std::fprintf(stderr, "cannot read SPIR-V from %s\n", argv[1]);
        return 1;
    }

    VkApplicationInfo app{};

    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "shader-math glsl interop";
    app.apiVersion = VK_API_VERSION_1_2;
    VkInstanceCreateInfo ici{};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &app;
    VkInstance instance;
    if (!ok(vkCreateInstance(&ici, nullptr, &instance), "vkCreateInstance")) return 1;

    uint32_t ndev = 0;
    vkEnumeratePhysicalDevices(instance, &ndev, nullptr);
    if (ndev == 0) {
        std::fprintf(stderr, "no Vulkan device (install mesa-vulkan-drivers for lavapipe)\n");
        return 1;
    }
    std::vector<VkPhysicalDevice> devs(ndev);
    vkEnumeratePhysicalDevices(instance, &ndev, devs.data());
    VkPhysicalDevice phys = devs[0];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(phys, &props);
    std::fprintf(stderr, "device: %s\n", props.deviceName);

    uint32_t nqf = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(phys, &nqf, nullptr);
    std::vector<VkQueueFamilyProperties> qfs(nqf);
    vkGetPhysicalDeviceQueueFamilyProperties(phys, &nqf, qfs.data());
    uint32_t qf = UINT32_MAX;
    for (uint32_t i = 0; i < nqf; ++i)
        if (qfs[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            qf = i;
            break;
        }
    if (qf == UINT32_MAX) {
        std::fprintf(stderr, "no compute queue\n");
        return 1;
    }

    // float16 arithmetic (core in Vulkan 1.2) only when the kernel needs it
    VkPhysicalDeviceShaderFloat16Int8Features f16{};
    f16.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES;
    if (want_f16) {
        VkPhysicalDeviceFeatures2 feats{};
        feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        feats.pNext = &f16;
        vkGetPhysicalDeviceFeatures2(phys, &feats);
        if (!f16.shaderFloat16) {
            std::fprintf(stderr, "SKIP: %s has no shaderFloat16\n", props.deviceName);
            return 77;
        }
        f16.shaderInt8 = VK_FALSE;
    }

    const float prio = 1.0f;
    VkDeviceQueueCreateInfo qci{};
    qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qci.queueFamilyIndex = qf;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;
    VkDeviceCreateInfo dci{};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    if (want_f16) dci.pNext = &f16;
    VkDevice dev;
    if (!ok(vkCreateDevice(phys, &dci, nullptr, &dev), "vkCreateDevice")) return 1;
    VkQueue queue;
    vkGetDeviceQueue(dev, qf, 0, &queue);

    // host-visible, coherent output buffer
    const VkDeviceSize size = sizeof(float) * kCases * 4;
    VkBufferCreateInfo bci{};
    bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bci.size = size;
    bci.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VkBuffer buf;
    if (!ok(vkCreateBuffer(dev, &bci, nullptr, &buf), "vkCreateBuffer")) return 1;
    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(dev, buf, &req);
    VkPhysicalDeviceMemoryProperties mp;
    vkGetPhysicalDeviceMemoryProperties(phys, &mp);
    const VkMemoryPropertyFlags want =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    uint32_t mt = UINT32_MAX;
    for (uint32_t i = 0; i < mp.memoryTypeCount; ++i)
        if ((req.memoryTypeBits & (1u << i)) && (mp.memoryTypes[i].propertyFlags & want) == want) {
            mt = i;
            break;
        }
    VkMemoryAllocateInfo mai{};
    mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mai.allocationSize = req.size;
    mai.memoryTypeIndex = mt;
    VkDeviceMemory mem;
    if (mt == UINT32_MAX || !ok(vkAllocateMemory(dev, &mai, nullptr, &mem), "vkAllocateMemory"))
        return 1;
    vkBindBufferMemory(dev, buf, mem, 0);

    VkDescriptorSetLayoutBinding bind{};
    bind.binding = 0;
    bind.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bind.descriptorCount = 1;
    bind.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    VkDescriptorSetLayoutCreateInfo dslci{};
    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dslci.bindingCount = 1;
    dslci.pBindings = &bind;
    VkDescriptorSetLayout dsl;
    vkCreateDescriptorSetLayout(dev, &dslci, nullptr, &dsl);
    VkPipelineLayoutCreateInfo plci{};
    plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plci.setLayoutCount = 1;
    plci.pSetLayouts = &dsl;
    VkPipelineLayout pl;
    vkCreatePipelineLayout(dev, &plci, nullptr, &pl);

    VkShaderModuleCreateInfo smci{};

    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = spv.size();
    smci.pCode = reinterpret_cast<const uint32_t*>(spv.data());
    VkShaderModule sm;
    if (!ok(vkCreateShaderModule(dev, &smci, nullptr, &sm), "vkCreateShaderModule")) return 1;
    VkComputePipelineCreateInfo cpci{};
    cpci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cpci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    cpci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cpci.stage.module = sm;
    cpci.stage.pName = "main";
    cpci.layout = pl;
    VkPipeline pipe;
    if (!ok(vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &cpci, nullptr, &pipe),
            "vkCreateComputePipelines"))
        return 1;

    VkDescriptorPoolSize ps{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1};
    VkDescriptorPoolCreateInfo dpci{};
    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpci.maxSets = 1;
    dpci.poolSizeCount = 1;
    dpci.pPoolSizes = &ps;
    VkDescriptorPool dp;
    vkCreateDescriptorPool(dev, &dpci, nullptr, &dp);
    VkDescriptorSetAllocateInfo dsai{};
    dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsai.descriptorPool = dp;
    dsai.descriptorSetCount = 1;
    dsai.pSetLayouts = &dsl;
    VkDescriptorSet ds;
    vkAllocateDescriptorSets(dev, &dsai, &ds);
    VkDescriptorBufferInfo dbi{buf, 0, size};
    VkWriteDescriptorSet w{};
    w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    w.dstSet = ds;
    w.dstBinding = 0;
    w.descriptorCount = 1;
    w.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    w.pBufferInfo = &dbi;
    vkUpdateDescriptorSets(dev, 1, &w, 0, nullptr);

    VkCommandPoolCreateInfo cpi{};

    cpi.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cpi.queueFamilyIndex = qf;
    VkCommandPool pool;
    vkCreateCommandPool(dev, &cpi, nullptr, &pool);
    VkCommandBufferAllocateInfo cbai{};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.commandPool = pool;
    cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbai.commandBufferCount = 1;
    VkCommandBuffer cb;
    vkAllocateCommandBuffers(dev, &cbai, &cb);
    VkCommandBufferBeginInfo cbbi{};
    cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cb, &cbbi);
    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_COMPUTE, pipe);
    vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_COMPUTE, pl, 0, 1, &ds, 0, nullptr);
    vkCmdDispatch(cb, 1, 1, 1);
    VkMemoryBarrier mb{};
    mb.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    mb.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    mb.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
    vkCmdPipelineBarrier(cb, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 1,
                         &mb, 0, nullptr, 0, nullptr);
    vkEndCommandBuffer(cb);
    VkSubmitInfo si{};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cb;
    if (!ok(vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE), "vkQueueSubmit")) return 1;
    vkQueueWaitIdle(queue);

    void* mapped = nullptr;
    vkMapMemory(dev, mem, 0, size, 0, &mapped);
    std::vector<float> out(kCases * 4);
    std::memcpy(out.data(), mapped, size);
    vkUnmapMemory(dev, mem);

    for (int c = 0; c < kCases; ++c) {
        for (int i = 0; i < 4; ++i) {
            std::uint32_t bits;
            std::memcpy(&bits, &out[c * 4 + i], sizeof(bits));
            std::printf("%s[%d] 0x%08x\n", kNames[c], i, bits);
        }
    }

    vkDestroyCommandPool(dev, pool, nullptr);
    vkDestroyDescriptorPool(dev, dp, nullptr);
    vkDestroyPipeline(dev, pipe, nullptr);
    vkDestroyShaderModule(dev, sm, nullptr);
    vkDestroyPipelineLayout(dev, pl, nullptr);
    vkDestroyDescriptorSetLayout(dev, dsl, nullptr);
    vkFreeMemory(dev, mem, nullptr);
    vkDestroyBuffer(dev, buf, nullptr);
    vkDestroyDevice(dev, nullptr);
    vkDestroyInstance(instance, nullptr);
    return 0;
}
