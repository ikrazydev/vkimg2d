#pragma once

#include <vulkan/vulkan.hpp>

class Device;

struct DeviceSwapchainConfig
{
    vk::SurfaceFormatKHR preferredFormat;
    vk::ColorSpaceKHR preferredColorSpace;
};

class DeviceSwapchain
{
public:
    DeviceSwapchain(Device& device, const DeviceSwapchainConfig& config);
private:
    vk::SurfaceFormatKHR _chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const;
    vk::PresentModeKHR _choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const;
    vk::Extent2D _chooseExtent2D(const vk::SurfaceCapabilitiesKHR& capabilities) const;

    Device& mDevice;

    vk::SurfaceFormatKHR mSurfaceFormat;
    vk::PresentModeKHR mPresentMode;
    vk::Extent2D mExtent2D;
};
