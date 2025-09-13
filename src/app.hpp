#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <optional>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> sValidationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char*> sDeviceExtensions = {
    "VK_KHR_portability_subset",
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

#if DEBUG
    const bool sEnableValidationLayers  = true;
#else
    const bool sEnableValidationLayers = false;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static auto getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static auto getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

const std::vector<Vertex> sVertices = {
    {{ -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }},
    {{ 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }},
    {{ 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }},
    {{ -1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }},
};

const std::vector<uint32_t> sIndices = {
    0, 1, 2, 2, 3, 0,
};

class VkImg2DApp {
public:
    void run();

    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

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
    bool verifyDeviceExtensionSupport(VkPhysicalDevice device);
    uint32_t getDeviceScore(VkPhysicalDevice device);
    void createLogicalDevice();

    void createSurface();

    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseExtent2D(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapchain();
    void createSwapchainImageViews();

    void createRenderPass();

    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& bytecode);
    void createGraphicsPipeline();
    void createFramebuffers();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createCommandPool();
    
    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& memory
    );
    void createVertexBuffer();
    void createIndexBuffer();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createTextureImage();
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void createSyncObjects();
    void createSwapchainImageSemaphores();
    void drawFrame();

    void cleanupSwapchain();
    void recreateSwapchain();
    
    void mainLoop();

    void cleanup();
private:
    GLFWwindow* mWindow;

    VkInstance mInstance;
    VkDebugUtilsMessengerEXT mDebugMessenger;

    VkSurfaceKHR mSurface;

    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;

    VkSwapchainKHR mSwapchain;
    std::vector<VkImage> mSwapchainImages;
    std::vector<VkImageView> mSwapchainImageViews;
    std::vector<VkFramebuffer> mSwapchainFramebuffers;
    VkFormat mSwapchainFormat;
    VkExtent2D mSwapchainExtent;

    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mPipeline;

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;

    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderedPerImageSemaphores;
    std::vector<VkFence> mInFlightFences;
    uint32_t mCurrentFrame = 0;
    bool mFramebufferResized = false;

    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;

    VkImage mTextureImage;
    VkDeviceMemory mTextureImageMemory;
};
