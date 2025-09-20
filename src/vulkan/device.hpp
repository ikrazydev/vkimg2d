#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <optional>

struct VkRendererConfig;
class Window;

struct DeviceQueueFamilies
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct _DeviceCreationResult
{
    vk::UniqueDevice device;

    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
};

class Device
{
public:
    void init(const VkRendererConfig& config, const vk::UniqueInstance& instance);

    ~Device();
private:
    void _createSurface(const vk::UniqueInstance& instance, const Window& window);

    vk::PhysicalDevice _pickPhysicalDevice(const vk::UniqueInstance& instance, const std::vector<const char*>& extensions);

    uint32_t _calculateDeviceScore(const vk::PhysicalDevice& device, const std::vector<const char*>& extensions);
    DeviceQueueFamilies _findQueueFamilies(const vk::PhysicalDevice& device);
    bool _verifyDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& extensions);

    _DeviceCreationResult _createLogicalDevice(const VkRendererConfig& config);

    vk::Instance mInstance;

    vk::SurfaceKHR mSurface;

    vk::PhysicalDevice mPhysicalDevice;
    DeviceQueueFamilies mQueueFamilies;

    vk::UniqueDevice mDevice;
    vk::Queue mGraphicsQueue;
    vk::Queue mPresentQueue;
};
