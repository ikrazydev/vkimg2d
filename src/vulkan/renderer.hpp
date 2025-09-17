#pragma once

#include <vulkan/vulkan.hpp>

struct VkRendererConfig
{
    uint32_t requiredExtensionCount;
    const char** requiredExtensions;
};

class VkRenderer
{
public:
    void init(VkRendererConfig config);
private:
    void _createInstance(const VkRendererConfig& config);

    vk::UniqueInstance mInstance;
};
