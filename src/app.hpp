#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#if DEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VkImg2DApp {
public:
    void run();

    void initWindow();

    void initVulkan();
    void createInstance();
    static void printExtensions();

    void setupDebugMessenger();
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static bool verifyValidationLayerSupport();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

    void pickPhysicalDevice();
    QueueFamilyIndices findDeviceFamilies(VkPhysicalDevice device);
    uint32_t getDeviceScore(VkPhysicalDevice device);
    void createLogicalDevice();

    void createSurface();
    
    void mainLoop();

    void cleanup();
private:
    GLFWwindow* mWindow;

    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;

    VkSurfaceKHR mSurface;

    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
};
