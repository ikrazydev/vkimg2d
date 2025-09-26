#pragma once

#include <vulkan/include.hpp>

#include <optional>

#include <vulkan/device.hpp>
#include <window.hpp>

struct VkRendererConfig
{
    const std::vector<const char*>& requiredExtensions;

    bool enableValidationLayers;
    const std::vector<const char*>& validationLayers;
    const std::vector<const char*>& deviceExtensions;

    const Window& window;
};

class VkRenderer
{
public:
    VkRenderer(VkRendererConfig config);
    ~VkRenderer();

    const vk::UniqueInstance& getInstance() const;
    const vk::SurfaceKHR getSurface() const;
private:
    void _createInstance(const VkRendererConfig& config);
    void _printExtensions();

    void _setupDebugMessenger();
    void _populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    void _createSurface(const Window& window);

    vk::UniqueInstance mInstance;
    vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;

    vk::SurfaceKHR mSurface;
    std::optional<Device> mDevice;
};
