#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

struct VkRendererConfig
{
    const char** requiredExtensions;
    uint32_t requiredExtensionCount;

    bool enableValidationLayers;
    const std::vector<const char*>& validationLayers;
};

class VkRenderer
{
public:
    void init(VkRendererConfig config);
private:
    void _createInstance(const VkRendererConfig& config);
    void _printExtensions();

    void _setupDebugMessenger();
    void _populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    vk::UniqueInstance mInstance;
    vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;
};
