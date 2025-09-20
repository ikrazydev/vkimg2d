#include "renderer.hpp"

#include <iostream>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

void VkRenderer::init(VkRendererConfig config)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    _createInstance(config);
    _printExtensions();

    if (config.enableValidationLayers) {
        _setupDebugMessenger();
    }

    _initDevice(config);
}

void VkRenderer::_createInstance(const VkRendererConfig& config)
{
    vk::ApplicationInfo appInfo{};
    appInfo.setPApplicationName("VkImg2D");
    appInfo.setApplicationVersion(VK_MAKE_VERSION(0, 1, 0));
    appInfo.setPEngineName("Custom");
    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_4);

    vk::InstanceCreateInfo createInfo{};
    createInfo.setPApplicationInfo(&appInfo);
    createInfo.setFlags(vk::InstanceCreateFlags(VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR));
    createInfo.setPEnabledExtensionNames(config.requiredExtensions);

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (config.enableValidationLayers) {
        createInfo.setEnabledLayerCount(static_cast<uint32_t>(config.validationLayers.size()));
        createInfo.setPpEnabledLayerNames(config.validationLayers.data());

        _populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.setPNext(&debugCreateInfo);
    } else {
        createInfo.setEnabledLayerCount(0);
    }

    mInstance = vk::createInstanceUnique(createInfo, nullptr);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance.get());
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void*)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";

    return VK_FALSE;
}

void VkRenderer::_printExtensions()
{
    auto exts = vk::enumerateInstanceExtensionProperties();

    std::cout << "Available extensions:\n";

    for (const auto& ext : exts) {
        std::cout << "\t" << ext.extensionName << "\n";
    }
}

void VkRenderer::_setupDebugMessenger()
{
    vk::DebugUtilsMessengerCreateInfoEXT createInfo;
    _populateDebugMessengerCreateInfo(createInfo);

    mDebugMessenger = mInstance->createDebugUtilsMessengerEXTUnique(createInfo);
}

void VkRenderer::_populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo.setMessageSeverity(
        // No info bit
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );
    createInfo.setMessageType(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
    );
    createInfo.setPfnUserCallback(debugCallback);
    createInfo.setPUserData(nullptr);
}

void VkRenderer::_initDevice(const VkRendererConfig& config)
{
    mDevice.init(config, mInstance);
}
