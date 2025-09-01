#include "app.hpp"

#include <iostream>

static VkResult _createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void _destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void VkImg2DApp::run()
{
    initWindow();
    initVulkan();

    mainLoop();

    cleanup();
}

void VkImg2DApp::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(WIDTH, HEIGHT, "VkImage2D", nullptr, nullptr);
}

void VkImg2DApp::initVulkan()
{
    createInstance();
    setupDebugMessenger();
    printExtensions();

    pickPhysicalDevice();
    createLogicalDevice();
}

void VkImg2DApp::createInstance()
{
    if (enableValidationLayers && !verifyValidationLayerSupport()) {
        throw std::runtime_error("Requested validation layers are not available.");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VkImg2D";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "None";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtCount = 0;
    const char** glfwExts;

    glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    std::vector<const char*> exts(glfwExts, glfwExts + glfwExtCount);
    exts.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        exts.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);

        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(exts.size());
    createInfo.ppEnabledExtensionNames = exts.data();

    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
}

void VkImg2DApp::printExtensions()
{
    uint32_t extCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> exts(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, exts.data());

    std::cout << "Available extensions:\n";

    for (const auto& ext : exts) {
        std::cout << "\t" << ext.extensionName << "\n";
    }
}

void VkImg2DApp::pickPhysicalDevice()
{
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceScore = 0;

    uint32_t count = 0;
    vkEnumeratePhysicalDevices(mInstance, &count, nullptr);

    if (count == 0) {
        throw std::runtime_error("No found GPUs for Vulkan.");
    }

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(mInstance, &count, devices.data());

    std::cout << "GPU devices:\n";

    for (const auto& device : devices) {
        std::cout << device << "\n";
        auto score = getDeviceScore(device);

        if (score > deviceScore) {
            physicalDevice = device;
            deviceScore = score;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("No suitable GPU found.");
    }

    mPhysicalDevice = physicalDevice;
}

QueueFamilyIndices VkImg2DApp::findDeviceFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices{};

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());

    for (uint32_t i = 0; i < families.size(); i++) {
        const auto& family = families[i];
        if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

uint32_t VkImg2DApp::getDeviceScore(VkPhysicalDevice device)
{
    uint32_t score = 0;

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(device, &props);

    switch (props.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        score += 100;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        score += 99;
        break;
    default:
        break;
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    auto families = findDeviceFamilies(device);

    if (!families.isComplete()) {
        return 0;
    }

    return score;
}

void VkImg2DApp::createLogicalDevice()
{
}

void VkImg2DApp::setupDebugMessenger()
{
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    auto result = _createDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger.");
    }
}

void VkImg2DApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = // No info bit
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

bool VkImg2DApp::verifyValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> vkLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, vkLayers.data());

    return std::ranges::all_of(validationLayers, [&](const char* layer) {
        return std::ranges::any_of(vkLayers, [&](const auto& vkLayer) {
            std::cout << std::format("Layer: {}\n", vkLayer.layerName);
            return std::strcmp(layer, vkLayer.layerName) == 0;
        });
    });
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkImg2DApp::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void*)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";

    return VK_FALSE;
}

void VkImg2DApp::mainLoop()
{
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();
    }
}

void VkImg2DApp::cleanup()
{
    if (enableValidationLayers) {
        _destroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
    }

    vkDestroyInstance(mInstance, nullptr);

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}
