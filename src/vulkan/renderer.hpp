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
#include <vulkan/pipeline/compute_pipeline.hpp>
#include <vulkan/pipeline/graphics_pipeline.hpp>
#include <vulkan/sync/fence.hpp>
#include <vulkan/sync/semaphore.hpp>
#include <imgui_renderer.hpp>
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
    
    Window& window;
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
    void _createTextures(const VkRendererConfig& config);
    void _createDescriptorLayouts(const VkRendererConfig& config);
    void _createDescriptorSets(const VkRendererConfig& config);
    void _createPipelines();
    void _setupImGui(const VkRendererConfig& config);
    void _createCommandBuffers(const VkRendererConfig& config);
    void _createSyncObjects(const VkRendererConfig& config);

    void _recreateSwapchain();

    Window& mWindow;

    vk::UniqueInstance mInstance;
    vk::UniqueDebugUtilsMessengerEXT mDebugMessenger;
    std::optional<GLFWVkSurface> mSurface;

    std::optional<Device> mDevice;

    std::optional<Renderpass> mRenderpass;

    std::vector<Framebuffer> mFramebuffers;

    uint32_t mCurrentFrame;
    uint32_t mFramesInFlight;

    std::optional<CommandPool> mCommandPool;

    std::optional<Buffer> mVertexBuffer;
    std::optional<Buffer> mIndexBuffer;

    std::optional<TextureImage> mTexture;
    std::optional<Sampler> mSampler;

    std::optional<DescriptorLayout> mFragmentDescriptorLayout;
    std::optional<DescriptorLayout> mSamplerDescriptorLayout;
    std::optional<DescriptorLayout> mGrayscaleDescriptorLayout;

    std::optional<DescriptorPool> mDescriptorPool;
    std::optional<DescriptorSet> mDescriptorSet;

    std::optional<ImGuiRenderer> mImGuiRenderer;

    std::optional<CommandBuffer> mCommandBuffers;

    std::optional<ComputePipeline> mSamplerPipeline;
    std::optional<ComputePipeline> mGrayscalePipeline;
    std::optional<GraphicsPipeline> mGraphicsPipeline;

    std::optional<BatchedSemaphores> mImageAvailableSemaphores;
    std::optional<BatchedSemaphores> mRenderedPerImageSemaphores;
    std::optional<BatchedFences> mInFlightFences;
};
