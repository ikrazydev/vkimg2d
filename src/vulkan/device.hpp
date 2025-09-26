#pragma once

#include <vulkan/include.hpp>
#include <vulkan/swapchain.hpp>

#include <optional>
#include <vector>

class VkRenderer;
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

struct DeviceSwapchainDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class Device
{
public:
    Device(const VkRendererConfig& config, VkRenderer& renderer);

    DeviceSwapchainDetails querySwapchainDetails(vk::PhysicalDevice device) const;
    DeviceSwapchainDetails querySwapchainDetails() const;

    vk::UniqueSwapchainKHR createSwapchainKHR(const vk::SwapchainCreateInfoKHR info) const;

    const Window& getWindow() const;
    const vk::SurfaceKHR getSurface() const;
    const DeviceQueueFamilies& getQueueFamilies() const;
    const vk::Device getVkDevice() const;
private:
    void _createSurface(const Window& window);

    vk::PhysicalDevice _pickPhysicalDevice(const std::vector<const char*>& extensions);

    uint32_t _calculateDeviceScore(const vk::PhysicalDevice& device, const std::vector<const char*>& extensions);
    DeviceQueueFamilies _findQueueFamilies(const vk::PhysicalDevice& device) const;
    bool _verifyDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& extensions);

    _DeviceCreationResult _createLogicalDevice(const VkRendererConfig& config);

    VkRenderer& mRenderer;
    const vk::UniqueInstance& mInstance;
    const Window& mWindow;

    vk::PhysicalDevice mPhysicalDevice;
    DeviceQueueFamilies mQueueFamilies;

    vk::UniqueDevice mDevice;
    vk::Queue mGraphicsQueue;
    vk::Queue mPresentQueue;

    std::optional<DeviceSwapchain> mSwapchain;
};
