#include "swapchain.hpp"

#include <vulkan/device.hpp>
#include <window.hpp>

#include <limits>

DeviceSwapchain::DeviceSwapchain(Device& device, const DeviceSwapchainConfig& config)
    : mDevice{ device }
    , mConfig{ config }
{
    auto details = device.querySwapchainDetails();

    _chooseSurfaceFormat(details.formats);
    _choosePresentMode(details.presentModes);
    _chooseExtent(details.capabilities);

    _createSwapchain(details.capabilities);
    _createSwapchainImages(details.capabilities);
}

void DeviceSwapchain::_chooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
{
    for (const auto& format : formats) {
        if (format.format == mConfig.preferredFormat && format.colorSpace == mConfig.preferredColorSpace) {
            mSurfaceFormat = format;
            return;
        }
    }

    // TODO: backup format choosing can be improved 
    mSurfaceFormat = formats[0];
}

void DeviceSwapchain::_choosePresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
{
    bool foundImmediate = false;

    for (const auto& presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            mPresentMode = presentMode;
        }
        if (presentMode == vk::PresentModeKHR::eImmediate) {
            foundImmediate = true;
        }
    }

    mPresentMode = foundImmediate ? vk::PresentModeKHR::eImmediate : vk::PresentModeKHR::eFifo;
}

void DeviceSwapchain::_chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != (::std::numeric_limits<uint32_t>::max)()) {
        mExtent = capabilities.currentExtent;
        return;
    }

    uint32_t width, height;
    mDevice.getWindow().getFramebufferSize(&width, &height);

    mExtent = vk::Extent2D{width, height};

    mExtent.width = std::clamp(mExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    mExtent.height = std::clamp(mExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
}

uint32_t DeviceSwapchain::_getImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    return imageCount;
}

void DeviceSwapchain::_createSwapchain(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto imageCount = _getImageCount(capabilities);

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = mDevice.getSurface();
    createInfo.presentMode = mPresentMode;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = mSurfaceFormat.format;
    createInfo.imageColorSpace = mSurfaceFormat.colorSpace;
    createInfo.imageExtent = mExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    const auto& families = mDevice.getQueueFamilies();
    uint32_t familyIndices[] = { families.graphicsFamily.value(), families.presentFamily.value() };

    if (families.graphicsFamily != families.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = familyIndices;
    }
    else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

    createInfo.clipped = vk::True;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    mSwapchain = mDevice.createSwapchainKHR(createInfo);
}

void DeviceSwapchain::_createSwapchainImages(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto vkDevice = mDevice.getVkHandle();

    mSwapchainImages = vkDevice.getSwapchainImagesKHR(mSwapchain.get());
    mSwapchainImageViews.resize(mSwapchainImages.size());

    for (size_t i = 0; i < mSwapchainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.image = mSwapchainImages[i];
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = mSurfaceFormat.format;

        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;

        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.layerCount = 1;

        mSwapchainImageViews[i] = vkDevice.createImageViewUnique(createInfo);
    }
}
