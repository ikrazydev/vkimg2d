#include "commandbuffer.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/framebuffer.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

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

void CommandBuffer::record(uint32_t currentFrame, uint32_t imageIndex)
{
    const auto& buffer = mCommandBuffers[currentFrame];
    
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlags{});
    beginInfo.setPInheritanceInfo(nullptr);

    buffer->begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(mConfig.renderpass.getVkHandle());
    renderPassInfo.setFramebuffer((*mConfig.framebuffers)[imageIndex].getVkHandle());
    renderPassInfo.renderArea.setOffset({ 0u, 0u });
    renderPassInfo.renderArea.setExtent(mConfig.extent);

    vk::ClearValue clearValue{ { 0.0f, 0.0f, 0.0f, 1.0f } };
    renderPassInfo.setClearValues({ clearValue });

    buffer->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, mConfig.graphicsPipeline.getVkHandle());

    vk::Buffer vertexBuffers[] = { mConfig.vertexBuffer.getVkHandle()};
    vk::DeviceSize offsets[] = { 0U };
    buffer->bindVertexBuffers(0U, vertexBuffers, offsets);

    buffer->bindIndexBuffer(mConfig.indexBuffer.getVkHandle(), 0U, vk::IndexType::eUint32);

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

    const auto descriptorSet = mConfig.graphicsDescSet.getVkHandle(currentFrame);
    buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mConfig.graphicsPipeline.getLayout(), 0U, descriptorSet, nullptr);

    buffer->drawIndexed(mConfig.drawIndexCount, mConfig.drawInstanceCount, 0U, 0U, 0U);

    recordImGui(currentFrame, imageIndex);

    buffer->endRenderPass();
    buffer->end();
}

void CommandBuffer::reset(uint32_t bufferIndex)
{
    const auto& buffer = mCommandBuffers[bufferIndex];
    buffer->reset();
}

void CommandBuffer::recordImGui(uint32_t currentFrame, uint32_t imageIndex)
{
    const auto& buffer = mCommandBuffers[currentFrame];
    
    ImGui::Render();
    auto* imGuiData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(imGuiData, buffer.get());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void CommandBuffer::updateFramebuffers(const std::vector<Framebuffer>* framebuffers, vk::Extent2D extent)
{
    mConfig.framebuffers = framebuffers;
    mConfig.extent = extent;
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
