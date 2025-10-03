#pragma once

#include <vulkan/include.hpp>
#include <vulkan/buffer/commandpool.hpp>
#include <vulkan/pipeline/graphics_pipeline.hpp>

#include <vector>

class Device;
class Renderpass;
class Framebuffer;

struct CommandBufferConfig
{
    const CommandPool& commandPool;
    const Renderpass& renderpass;
    const Framebuffer& framebuffer;
    const GraphicsPipeline& pipeline;
    
    vk::Extent2D extent;

    uint32_t drawVertexCount;
    uint32_t drawInstanceCount;
};

class CommandBuffer
{
public:
    CommandBuffer(const Device& device, const CommandBufferConfig& config);
private:
    void _createCommandBuffer();
    void _record();

    const Device& mDevice;
    const CommandBufferConfig& mConfig;
    
    std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};
