#include "commandbuffer.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/framebuffer.hpp>

#include <imgui.h>
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
    const auto& buffer = mCommandBuffers.at(currentFrame);
    auto& renderImages = mConfig.renderImages.at(currentFrame);

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
    renderPassInfo.setClearValues(clearValue);

    buffer->beginRenderPass2(renderPassInfo, vk::SubpassContents::eInline);

    // Sampler pipeline
    buffer->bindPipeline(vk::PipelineBindPoint::eCompute, mConfig.computePipeline.getVkHandle());

    auto samplerDescSet = mConfig.renderDescriptors.sampler.getVkHandle(currentFrame);
    vk::BindDescriptorSetsInfo samplerBindInfo{};
    samplerBindInfo.setStageFlags(vk::ShaderStageFlagBits::eCompute);
    samplerBindInfo.setLayout(mConfig.computePipeline.getLayout());
    samplerBindInfo.setDescriptorSets(samplerDescSet);
    samplerBindInfo.setFirstSet(0U);
    samplerBindInfo.setDynamicOffsets(nullptr);
    buffer->bindDescriptorSets2(samplerBindInfo);

    uint32_t groupsX = (renderImages.original.getWidth() + 15U) / 16U;
    uint32_t groupsY = (renderImages.original.getHeight() + 15U) / 16U;
    buffer->dispatch(groupsX, groupsY, 1U);

    renderImages.ping.transitionComputeToFragmentRead(buffer.get());

    // Graphics pipeline
    buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, mConfig.graphicsPipeline.getVkHandle());

    vk::Buffer vertexBuffers[] = { mConfig.vertexBuffer.getVkHandle()};
    vk::DeviceSize offsets[] = { 0U };
    buffer->bindVertexBuffers2(0U, vertexBuffers, offsets);

    buffer->bindIndexBuffer2(mConfig.indexBuffer.getVkHandle(), 0U, vk::WholeSize, vk::IndexType::eUint32);

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

    // read the sampled image; TODO: switch to grayscale processing later
    auto descriptorSet = mConfig.renderDescriptors.graphicsA.getVkHandle(currentFrame);
    vk::BindDescriptorSetsInfo graphicsBindInfo{};
    samplerBindInfo.setStageFlags(vk::ShaderStageFlagBits::eAllGraphics);
    samplerBindInfo.setLayout(mConfig.graphicsPipeline.getLayout());
    samplerBindInfo.setDescriptorSets(descriptorSet);
    samplerBindInfo.setFirstSet(0U);
    samplerBindInfo.setDynamicOffsets(nullptr);
    buffer->bindDescriptorSets2(graphicsBindInfo);

    std::array pushValues = { 1.0f };
    vk::PushConstantsInfo pushConstInfo{};
    pushConstInfo.setLayout(mConfig.graphicsPipeline.getLayout());
    pushConstInfo.setStageFlags(vk::ShaderStageFlagBits::eFragment);
    pushConstInfo.setOffset(0U);
    pushConstInfo.setValues<float>(pushValues);
    buffer->pushConstants2(pushConstInfo);

    renderImages.ping.transitionComputeToFragmentRead(buffer.get());

    buffer->drawIndexed(mConfig.drawIndexCount, mConfig.drawInstanceCount, 0U, 0U, 0U);

    renderImages.ping.transitionRevertToCompute(buffer.get());

    recordImGui(currentFrame, imageIndex);

    buffer->endRenderPass2(vk::SubpassEndInfo{});
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
