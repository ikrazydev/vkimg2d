#include "device.hpp"

#include <vulkan/renderer.hpp>
#include <vulkan/swapchain.hpp>
#include <window.hpp>

#include <iostream>
#include <vector>
#include <set>
#include <stdexcept>

Device::Device(const VkRendererConfig& config, VkRenderer& renderer)
    : mRenderer(renderer)
    , mInstance{mRenderer.getInstance()}
    , mWindow{config.window}
{
    mPhysicalDevice = _pickPhysicalDevice(config.deviceExtensions);
    mQueueFamilies = _findQueueFamilies(mPhysicalDevice);

    auto deviceCreationResult = _createLogicalDevice(config);
    mDevice = std::move(deviceCreationResult.device);
    mGraphicsQueue = deviceCreationResult.graphicsQueue;
    mPresentQueue = deviceCreationResult.presentQueue;
    mComputeQueue = deviceCreationResult.computeQueue;

    recreateSwapchain();
}

vk::PhysicalDevice Device::_pickPhysicalDevice(const std::vector<const char*>& extensions)
{
    vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceScore = 0;

    auto devices = mInstance->enumeratePhysicalDevices();

    if (devices.size() == 0) {
        throw std::runtime_error("No found GPUs for Vulkan.");
    }

    std::cout << "GPU devices:\n";

    for (const auto& device : devices) {
        auto props = device.getProperties();

        std::cout << props.deviceName << "\n";
        auto score = _calculateDeviceScore(device, extensions);

        if (score > deviceScore) {
            physicalDevice = device;
            deviceScore = score;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable GPU found.");
    }

    std::cout << "Chosen GPU: " << physicalDevice.getProperties().deviceName << "\n";

    return physicalDevice;
}

uint32_t Device::_calculateDeviceScore(const vk::PhysicalDevice& device, const std::vector<const char*>& extensions)
{
    uint32_t score = 0;

    auto props = device.getProperties();

    score += props.limits.maxColorAttachments;
    score += static_cast<uint32_t>(props.limits.maxSamplerAnisotropy);

    switch (props.deviceType)
    {
    case vk::PhysicalDeviceType::eDiscreteGpu:
        score += 1000;
        break;
    case vk::PhysicalDeviceType::eIntegratedGpu:
        score += 990;
        break;
    default:
        break;
    }

    auto features = device.getFeatures();
    if (!features.samplerAnisotropy) {
        return 0;
    }

    auto families = _findQueueFamilies(device);
    if (!families.isComplete()) {
        return 0;
    }
    if (!_verifyDeviceExtensionSupport(device, extensions)) {
        return 0;
    }

    auto swapchainDetails = querySwapchainDetails(device);

    if (swapchainDetails.formats.empty() || swapchainDetails.presentModes.empty()) {
        return 0;
    }

    return score;
}

bool Device::_verifyDeviceExtensionSupport(vk::PhysicalDevice device, const std::vector<const char*>& extensions)
{
    auto exts = device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExts(extensions.begin(), extensions.end());

    for (const auto& ext : exts) {
        requiredExts.erase(ext.extensionName);
    }

    return requiredExts.empty();
}

DeviceSwapchainDetails Device::querySwapchainDetails(vk::PhysicalDevice device) const
{
    auto surface = getSurface();

    auto capabilities = device.getSurfaceCapabilitiesKHR(surface);
    auto formats = device.getSurfaceFormatsKHR(surface);
    auto presentModes = device.getSurfacePresentModesKHR(surface);

    return DeviceSwapchainDetails{
        .capabilities = std::move(capabilities),
        .formats = std::move(formats),
        .presentModes = std::move(presentModes),
    };
}

DeviceSwapchainDetails Device::querySwapchainDetails() const
{
    return querySwapchainDetails(mPhysicalDevice);
}

const Window& Device::getWindow() const
{
    return mWindow;
}

const vk::PhysicalDevice Device::getPhysicalDevice() const
{
    return mPhysicalDevice;
}

const vk::SurfaceKHR Device::getSurface() const
{
    return mRenderer.getSurface();
}

const DeviceQueueFamilies& Device::getQueueFamilies() const
{
    return mQueueFamilies;
}

const DeviceSwapchain& Device::getSwapchain() const
{
    return mSwapchain.value();
}

const vk::Queue& Device::getGraphicsQueue() const
{
    return mGraphicsQueue;
}

const vk::Queue& Device::getPresentQueue() const
{
    return mPresentQueue;
}

const vk::Device Device::getVkHandle() const
{
    return mDevice.get();
}

DeviceQueueFamilies Device::_findQueueFamilies(const vk::PhysicalDevice& device) const
{
    DeviceQueueFamilies indices{};

    auto surface = getSurface();
    auto families = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < families.size(); i++) {
        const auto& family = families[i];
        if ((family.queueFlags & vk::QueueFlagBits::eGraphics) && (family.queueFlags & vk::QueueFlagBits::eCompute)) {
            indices.graphicsAndComputeFamily = i;
        }

        auto presentSupport = device.getSurfaceSupportKHR(i, surface);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

_DeviceCreationResult Device::_createLogicalDevice(const VkRendererConfig& config)
{
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set uniqueQueueFamilies = { mQueueFamilies.graphicsAndComputeFamily.value(), mQueueFamilies.presentFamily.value() };

    float queuePriority = 1.0f;
    for (auto queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.setQueueFamilyIndex(queueFamily);
        queueCreateInfo.setQueueCount(1);
        queueCreateInfo.setQueuePriorities(queuePriority);

        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.setSamplerAnisotropy(vk::True);

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos);
    deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);

    deviceCreateInfo.setPEnabledExtensionNames(config.deviceExtensions);

    deviceCreateInfo.setEnabledLayerCount(0);
    if (config.enableValidationLayers) {
        deviceCreateInfo.setPEnabledLayerNames(config.validationLayers);
    }

    auto device = mPhysicalDevice.createDeviceUnique(deviceCreateInfo);

    auto graphicsQueue = device->getQueue(mQueueFamilies.graphicsAndComputeFamily.value(), 0);
    auto presentQueue = device->getQueue(mQueueFamilies.presentFamily.value(), 0);
    auto computeQueue = device->getQueue(mQueueFamilies.graphicsAndComputeFamily.value(), 0);

    return _DeviceCreationResult{
        .device = std::move(device),

        .graphicsQueue = graphicsQueue,
        .presentQueue = presentQueue,
        .computeQueue = computeQueue,
    };
}

vk::UniqueSwapchainKHR Device::createSwapchainKHR(const vk::SwapchainCreateInfoKHR info) const
{
    auto result = mDevice->createSwapchainKHRUnique(info);

    return result;
}

vk::ResultValue<uint32_t> Device::acquireNextImageKHR(vk::Semaphore semaphore, uint64_t timeout) const
{
    return mDevice->acquireNextImageKHR(mSwapchain->getVkHandle(), timeout, semaphore);
}

void Device::recreateSwapchain()
{
    mSwapchain.reset();

    DeviceSwapchainConfig swapchainConfig{
        .preferredFormat = vk::Format::eB8G8R8A8Srgb,
        .preferredColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
    };

    mSwapchain.emplace(*this, swapchainConfig);
}
