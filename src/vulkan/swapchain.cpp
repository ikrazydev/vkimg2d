#include "swapchain.hpp"

#include <array>
#include <limits>

#include <vulkan/device.hpp>
#include <window.hpp>

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

uint32_t DeviceSwapchain::getImageCount() const
{
    return static_cast<uint32_t>(mSwapchainImages.size());
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

uint32_t DeviceSwapchain::_queryImageCount(const vk::SurfaceCapabilitiesKHR& capabilities) const
{
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    return imageCount;
}

void DeviceSwapchain::_createSwapchain(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto imageCount = _queryImageCount(capabilities);

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.setSurface(mDevice.getSurface());
    createInfo.setPresentMode(mPresentMode);
    createInfo.setMinImageCount(imageCount);
    createInfo.setImageFormat(mSurfaceFormat.format);
    createInfo.setImageColorSpace(mSurfaceFormat.colorSpace);
    createInfo.setImageExtent(mExtent);
    createInfo.setImageArrayLayers(1);
    createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

    const auto& families = mDevice.getQueueFamilies();
    std::array familyIndices{ families.graphicsFamily.value(), families.presentFamily.value() };

    if (families.graphicsFamily != families.presentFamily) {
        createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
        createInfo.setQueueFamilyIndices(familyIndices);
    }
    else {
        createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
        createInfo.setQueueFamilyIndices(nullptr);
    }

    createInfo.setPreTransform(capabilities.currentTransform);
    createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

    createInfo.setClipped(vk::True);
    createInfo.setOldSwapchain(VK_NULL_HANDLE);

    mSwapchain = mDevice.createSwapchainKHR(createInfo);
}

void DeviceSwapchain::_createSwapchainImages(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto vkDevice = mDevice.getVkHandle();

    mSwapchainImages = vkDevice.getSwapchainImagesKHR(mSwapchain.get());
    mSwapchainImageViews.resize(mSwapchainImages.size());

    for (size_t i = 0; i < mSwapchainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.setImage(mSwapchainImages[i]);
        createInfo.setViewType(vk::ImageViewType::e2D);
        createInfo.setFormat(mSurfaceFormat.format);

        createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
        createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
        createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
        createInfo.components.setA(vk::ComponentSwizzle::eIdentity);

        createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        createInfo.subresourceRange.setBaseMipLevel(0U);
        createInfo.subresourceRange.setBaseArrayLayer(0U);
        createInfo.subresourceRange.setLevelCount(1U);
        createInfo.subresourceRange.setLayerCount(1U);

        mSwapchainImageViews[i] = vkDevice.createImageViewUnique(createInfo);
    }
}
