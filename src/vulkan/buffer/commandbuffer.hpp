#pragma once

#include <vulkan/include.hpp>
#include <vulkan/buffer/commandpool.hpp>
#include <vulkan/buffer/texture.hpp>
#include <vulkan/descriptor/descriptor_set.hpp>
#include <vulkan/pipeline/compute_pipeline.hpp>
#include <vulkan/pipeline/graphics_pipeline.hpp>

#include <vector>

class Buffer;
class Device;
class Renderpass;
class Framebuffer;

struct RenderDescriptorSets
{
    DescriptorSet sampler;

    DescriptorSet computeAtoB;
    DescriptorSet computeBtoA;

    DescriptorSet graphicsA;
    DescriptorSet graphicsB;
};

struct RenderImageSet
{
    const TextureImage& original;
    TextureImage ping;
    TextureImage pong;
};

struct CommandBufferConfig
{
    const CommandPool& commandPool;
    const Renderpass& renderpass;
    const std::vector<Framebuffer>* framebuffers;

    const RenderDescriptorSets& renderDescriptors;
    std::vector<RenderImageSet>& renderImages;
    const GraphicsPipeline& graphicsPipeline;
    const ComputePipeline& computePipeline;

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
    CommandBuffer(const Device& device, const CommandBufferConfig& config);

    void record(uint32_t currentFrame, uint32_t imageIndex);
    void reset(uint32_t bufferIndex);

    void recordImGui(uint32_t currentFrame, uint32_t imageIndex);

    void updateFramebuffers(const std::vector<Framebuffer>* framebuffers, vk::Extent2D extent);

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
