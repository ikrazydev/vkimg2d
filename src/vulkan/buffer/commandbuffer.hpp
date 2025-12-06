#pragma once

#include <vulkan/include.hpp>
#include <vulkan/buffer/commandpool.hpp>
#include <vulkan/descriptor/descriptor_set.hpp>
#include <vulkan/pipeline/graphics_pipeline.hpp>

#include <vector>

class Buffer;
class Device;
class Renderpass;
class Framebuffer;

struct CommandBufferConfig
{
    const CommandPool& commandPool;
    const Renderpass& renderpass;
    const std::vector<Framebuffer>& framebuffers;
    const DescriptorSet& descriptorSet;
    const GraphicsPipeline& pipeline;
    
    vk::Extent2D extent;

    uint32_t createCount;

    const Buffer& vertexBuffer;
    const Buffer& indexBuffer;

    uint32_t drawIndexCount;
    uint32_t drawInstanceCount;
};

class CommandBuffer
{
public:
    CommandBuffer(const Device& device, CommandBufferConfig config);

    void record(size_t bufferIndex);
    void reset(size_t bufferIndex);

    [[nodiscard]] const vk::CommandBuffer getVkHandle(size_t bufferIndex) const noexcept;
private:
    const Device& mDevice;
    CommandBufferConfig mConfig;
    
    std::vector<vk::UniqueCommandBuffer> mCommandBuffers;
};

class SingleTimeCommandBuffer
{
public:
    SingleTimeCommandBuffer(const Device& device, const CommandPool& commandPool);
    ~SingleTimeCommandBuffer();

    const vk::CommandBuffer getVkHandle() const;
private:
    const Device& mDevice;

    vk::UniqueCommandBuffer mCommandBuffer;
};
