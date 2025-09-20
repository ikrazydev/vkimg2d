#include "device.hpp"

#include <vulkan/renderer.hpp>
#include <window.hpp>

#include <iostream>
#include <vector>
#include <set>
#include <stdexcept>

void Device::init(const VkRendererConfig& config, const vk::UniqueInstance& instance)
{
    mInstance = instance.get();

    _createSurface(instance, config.window);

    mPhysicalDevice = _pickPhysicalDevice(instance, config.deviceExtensions);
    mQueueFamilies = _findQueueFamilies(mPhysicalDevice);

    auto deviceCreationResult = _createLogicalDevice(config);
    mDevice = std::move(deviceCreationResult.device);
    mGraphicsQueue = deviceCreationResult.graphicsQueue;
    mPresentQueue = deviceCreationResult.presentQueue;
}

Device::~Device()
{
    if (mInstance == VK_NULL_HANDLE) {
        return;
    }

    mInstance.destroySurfaceKHR(mSurface);
}

void Device::_createSurface(const vk::UniqueInstance& instance, const Window& window)
{
    VkSurfaceKHR rawSurface;
    if (window.vkCreateSurface(instance.get(), nullptr, &rawSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }

    mSurface = vk::SurfaceKHR(rawSurface);
}

vk::PhysicalDevice Device::_pickPhysicalDevice(const vk::UniqueInstance& instance, const std::vector<const char*>& extensions)
{
    vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceScore = 0;

    auto devices = instance->enumeratePhysicalDevices();

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

    // auto swapchainSupport = querySwapchainSupport(device);

    // if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
    //     return 0;
    // }

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

DeviceQueueFamilies Device::_findQueueFamilies(const vk::PhysicalDevice& device)
{
    DeviceQueueFamilies indices{};

    auto families = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < families.size(); i++) {
        const auto& family = families[i];
        if (family.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }

        auto presentSupport = device.getSurfaceSupportKHR(i, mSurface);
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
    std::set uniqueQueueFamilies = { mQueueFamilies.graphicsFamily.value(), mQueueFamilies.presentFamily.value() };

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

    auto graphicsQueue = device->getQueue(mQueueFamilies.graphicsFamily.value(), 0);
    auto presentQueue = device->getQueue(mQueueFamilies.presentFamily.value(), 0);

    return _DeviceCreationResult{
        .device = std::move(device),

        .graphicsQueue = graphicsQueue,
        .presentQueue = presentQueue,
    };
}
