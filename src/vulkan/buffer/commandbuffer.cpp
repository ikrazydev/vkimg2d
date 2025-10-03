#include "commandbuffer.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/buffer/framebuffer.hpp>

CommandBuffer::CommandBuffer(const Device& device, const CommandBufferConfig& config)
    : mDevice{ device }
    , mConfig{ config }
{
    _createCommandBuffer();
}

void CommandBuffer::_createCommandBuffer()
{
    vk::CommandBufferAllocateInfo allocateInfo{};
    allocateInfo.setCommandPool(mConfig.commandPool.getVkHandle());
    allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocateInfo.setCommandBufferCount(1U);

    mCommandBuffers = mDevice.getVkHandle().allocateCommandBuffersUnique(allocateInfo);
}

void CommandBuffer::_record()
{
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlags{};
    beginInfo.pInheritanceInfo = nullptr;

    mCommandBuffers[0]->begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.renderPass = mConfig.renderpass.getVkHandle();
    renderPassInfo.framebuffer = mConfig.framebuffer.getVkHandle();
    renderPassInfo.renderArea.offset = vk::Offset2D{ 0U, 0U };
    renderPassInfo.renderArea.extent = mConfig.extent;

    vk::ClearValue clearValue{ { 0.0f, 0.0f, 0.0f, 1.0f } };
    renderPassInfo.setPClearValues(&clearValue);
    renderPassInfo.setClearValueCount(1U);

    mCommandBuffers[0]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    mCommandBuffers[0]->bindPipeline(vk::PipelineBindPoint::eGraphics, mConfig.pipeline.getVkHandle());

    vk::Viewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)mConfig.extent.width;
    viewport.height = (float)mConfig.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    mCommandBuffers[0]->setViewport(0, { viewport });

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{ 0, 0 };
    scissor.extent = mConfig.extent;
    mCommandBuffers[0]->setScissor(0, { scissor });

    mCommandBuffers[0]->draw(mConfig.drawVertexCount, mConfig.drawInstanceCount, 0U, 0U);

    mCommandBuffers[0]->endRenderPass();

    mCommandBuffers[0]->end();
}
