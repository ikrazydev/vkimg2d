#include "commandbuffer.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/buffer/framebuffer.hpp>

std::vector<vk::UniqueCommandBuffer> createCommandBuffers(const vk::Device device, const vk::CommandPool pool, uint32_t createCount)
{
    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.setCommandPool(pool);
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocateInfo.setCommandBufferCount(createCount);

    return device.allocateCommandBuffersUnique(allocateInfo);
}

CommandBuffer::CommandBuffer(const Device& device, const CommandBufferConfig& config)
    : mDevice{ device }
    , mConfig{ config }
{
    mCommandBuffers = createCommandBuffers(device.getVkHandle(), config.commandPool.getVkHandle(), config.createCount);
}

void CommandBuffer::record(size_t bufferIndex)
{
    const auto& buffer = mCommandBuffers[bufferIndex];
    
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlags{});
    beginInfo.setPInheritanceInfo(VK_NULL_HANDLE);

    buffer->begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(mConfig.renderpass.getVkHandle());
    renderPassInfo.setFramebuffer(mConfig.framebuffer.getVkHandle());
    renderPassInfo.renderArea.setOffset({ 0u, 0u });
    renderPassInfo.renderArea.setExtent(mConfig.extent);

    vk::ClearValue clearValue{ { 0.0f, 0.0f, 0.0f, 1.0f } };
    renderPassInfo.setClearValues({ clearValue });

    buffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, mConfig.pipeline.getVkHandle());

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(mConfig.extent.width));
    viewport.setHeight(static_cast<float>(mConfig.extent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(0.0f);
    buffer->setViewport(0u, { viewport });

    vk::Rect2D scissor{};
    scissor.setOffset({ 0u, 0u });
    scissor.setExtent(mConfig.extent);
    buffer->setScissor(0u, { scissor });

    buffer->draw(mConfig.drawVertexCount, mConfig.drawInstanceCount, 0u, 0u);

    buffer->endRenderPass();

    buffer->end();
}

void CommandBuffer::reset(size_t bufferIndex)
{
    const auto& buffer = mCommandBuffers[bufferIndex];
    buffer->reset();
}

const vk::CommandBuffer CommandBuffer::getVkHandle(size_t bufferIndex) const noexcept
{
    return mCommandBuffers[bufferIndex].get();
}

SingleTimeCommandBuffer::SingleTimeCommandBuffer(const Device& device, const CommandPool& commandPool)
    : mDevice{ device }
{
    auto buffers = createCommandBuffers(device.getVkHandle(), commandPool.getVkHandle(), 1U);
    mCommandBuffer = std::move(buffers[0]);

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    mCommandBuffer->begin(beginInfo);
}

SingleTimeCommandBuffer::~SingleTimeCommandBuffer()
{
    mCommandBuffer->end();

    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(mCommandBuffer.get());

    const auto graphicsQueue = mDevice.getGraphicsQueue();
    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

const vk::CommandBuffer SingleTimeCommandBuffer::getVkHandle() const
{
    return mCommandBuffer.get();
}
