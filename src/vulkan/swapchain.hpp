#pragma once

#include <vulkan/include.hpp>

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

    [[nodiscard]] uint32_t getImageCount() const noexcept;
    [[nodiscard]] vk::SwapchainKHR getVkHandle() const noexcept;
private:
    void _chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);
    void _choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
    void _chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities);

    uint32_t _queryImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) const noexcept;
    void _createSwapchain(const vk::SurfaceCapabilitiesKHR& capabilities);
    void _createSwapchainImages(const vk::SurfaceCapabilitiesKHR& capabilities);

    const Device& mDevice;
    const DeviceSwapchainConfig& mConfig;

    vk::SurfaceFormatKHR mSurfaceFormat;
    vk::PresentModeKHR mPresentMode;
    vk::Extent2D mExtent;

    vk::UniqueSwapchainKHR mSwapchain;
    std::vector<vk::Image> mSwapchainImages;
    std::vector<vk::UniqueImageView> mSwapchainImageViews;
};
