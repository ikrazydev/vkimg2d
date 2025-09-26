#include "swapchain.hpp"

#include <vulkan/device.hpp>
#include <window.hpp>

#include <limits>

DeviceSwapchain::DeviceSwapchain(Device& device, const DeviceSwapchainConfig& config)
    : mDevice(device)
{
    auto details = device.querySwapchainDetails();

    mSurfaceFormat = _chooseSurfaceFormat(details.formats);
    mPresentMode = _choosePresentMode(details.presentModes);
    mExtent2D = _chooseExtent2D(details.capabilities);
}

vk::SurfaceFormatKHR DeviceSwapchain::_chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats) const
{
    for (const auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format;
        }
    }

    // TODO: backup format choosing can be improved 
    return formats[0];
}

vk::PresentModeKHR DeviceSwapchain::_choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes) const
{
    bool foundImmediate = false;

    for (const auto& presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            return presentMode;
        }
        if (presentMode == vk::PresentModeKHR::eImmediate) {
            foundImmediate = true;
        }
    }

    return foundImmediate ? vk::PresentModeKHR::eImmediate : vk::PresentModeKHR::eFifo;
}

vk::Extent2D DeviceSwapchain::_chooseExtent2D(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
    if (capabilities.currentExtent.width != (::std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    }

    uint32_t width, height;
    mDevice.getWindow().getFramebufferSize(&width, &height);

    vk::Extent2D extent{width, height};

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}
