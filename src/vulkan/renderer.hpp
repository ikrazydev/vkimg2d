#pragma once

#include <vulkan/include.hpp>

#include <optional>

#include <vulkan/device.hpp>
#include <vulkan/glfw_surface.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/sampler.hpp>
#include <vulkan/vertex.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/commandbuffer.hpp>
#include <vulkan/buffer/commandpool.hpp>
#include <vulkan/buffer/framebuffer.hpp>
#include <vulkan/buffer/texture.hpp>
#include <vulkan/descriptor/descriptor_layout.hpp>
#include <vulkan/descriptor/descriptor_pool.hpp>
#include <vulkan/descriptor/descriptor_set.hpp>
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

    const std::vector<Vertex>& vertices;
    const std::vector<uint32_t>& indices;

    uint32_t framesInFlight;
    
    const Window& window;
};

class VkRenderer
{
public:
    VkRenderer(VkRendererConfig config);

    const vk::UniqueInstance& getInstance() const;
    const vk::SurfaceKHR getSurface() const;

    void draw();

    void cleanup();
private:
    void _createInstance(const VkRendererConfig& config);
    void _printExtensions();

    void _setupDebugMessenger();
    void _populateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);

    void _createSurface(const Window& window);

    void _createRenderpass();
    void _createFramebuffers();
    void _createCommandPool();
    void _createBuffers(const VkRendererConfig& config);
    void _createTexture(const VkRendererConfig& config);
    void _createDescriptors(const VkRendererConfig& config);
    void _createGraphicsPipeline();
    void _setupImGui(const VkRendererConfig& config);
    void _createCommandBuffers(const VkRendererConfig& config);
    void _createSyncObjects(const VkRendererConfig& config);

    vk::UniqueInstance mInstance;
    vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;
    std::optional<GLFWVkSurface> mSurface;

    std::optional<Device> mDevice;

    std::optional<Renderpass> mRenderpass;
    std::optional<Renderpass> mImGuiRenderpass;

    std::vector<Framebuffer> mFramebuffers;

    uint32_t mCurrentFrame;
    uint32_t mFramesInFlight;

    std::optional<CommandPool> mCommandPool;

    std::optional<Buffer> mVertexBuffer;
    std::optional<Buffer> mIndexBuffer;

    std::optional<TextureImage> mTexture;
    std::optional<Sampler> mSampler;

    std::optional<DescriptorLayout> mDescriptorLayout;
    std::optional<DescriptorPool> mDescriptorPool;
    std::optional<DescriptorSet> mDescriptorSet;

    std::optional<CommandBuffer> mCommandBuffers;
    std::optional<CommandBuffer> mImGuiCommandBuffers;

    std::optional<GraphicsPipeline> mPipeline;

    std::optional<BatchedSemaphores> mImageAvailableSemaphores;
    std::optional<BatchedSemaphores> mRenderedPerImageSemaphores;
    std::optional<BatchedFences> mInFlightFences;
};
