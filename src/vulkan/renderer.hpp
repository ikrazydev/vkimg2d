#pragma once

#include <vulkan/include.hpp>

#include <optional>

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/buffer/commandbuffer.hpp>
#include <vulkan/buffer/commandpool.hpp>
#include <vulkan/buffer/framebuffer.hpp>
#include <vulkan/pipeline/graphics_pipeline.hpp>
#include <vulkan/sync/fence.hpp>
#include <vulkan/sync/semaphore.hpp>
#include <window.hpp>

struct VkRendererConfig
{
    const std::vector<const char*>& requiredExtensions;

    bool enableValidationLayers;
    const std::vector<const char*>& validationLayers;
    const std::vector<const char*>& deviceExtensions;

    const Window& window;

    uint32_t framesInFlight;
};

class VkRenderer
{
public:
    VkRenderer(VkRendererConfig config);
    ~VkRenderer();

    const vk::UniqueInstance& getInstance() const;
    const vk::SurfaceKHR getSurface() const;

    void draw();
private:
    void _createInstance(const VkRendererConfig& config);
    void _printExtensions();

    void _setupDebugMessenger();
    void _populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    void _createSurface(const Window& window);

    void _createRenderpass();
    void _createFramebuffers();
    void _createCommandPool();
    void _createGraphicsPipeline();
    void _createCommandBuffers(const VkRendererConfig& config);
    void _createSyncObjects(const VkRendererConfig& config);

    vk::UniqueInstance mInstance;
    vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;

    vk::SurfaceKHR mSurface;
    std::optional<Device> mDevice;
    std::optional<Renderpass> mRenderpass;
    std::vector<Framebuffer> mFramebuffers;

    uint32_t mCurrentFrame;
    uint32_t mFramesInFlight;

    std::optional<CommandPool> mCommandPool;
    std::optional<CommandBuffer> mCommandBuffers;

    std::optional<GraphicsPipeline> mPipeline;

    std::optional<BatchedSemaphores> mImageAvailableSemaphores;
    std::optional<BatchedSemaphores> mRenderedPerImageSemaphores;
    std::optional<BatchedFences> mInFlightFences;
};
