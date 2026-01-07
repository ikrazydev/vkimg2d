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
    const auto& renderDescriptors = mConfig.renderDescriptors.at(currentFrame);

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlags{});
    beginInfo.setPInheritanceInfo(nullptr);

    buffer->begin(beginInfo);

    // Sampler pipeline
    buffer->bindPipeline(vk::PipelineBindPoint::eCompute, mConfig.samplerPipeline.getVkHandle());

    auto samplerDescSet = renderDescriptors.sampler.getVkHandle();
    vk::BindDescriptorSetsInfo samplerBindInfo{};
    samplerBindInfo.setStageFlags(vk::ShaderStageFlagBits::eCompute);
    samplerBindInfo.setLayout(mConfig.samplerPipeline.getLayout());
    samplerBindInfo.setDescriptorSets(samplerDescSet);
    samplerBindInfo.setFirstSet(0U);
    samplerBindInfo.setDynamicOffsets(nullptr);
    buffer->bindDescriptorSets2(samplerBindInfo);

    uint32_t groupsX = (renderImages.original.getWidth() + 15U) / 16U;
    uint32_t groupsY = (renderImages.original.getHeight() + 15U) / 16U;
    buffer->dispatch(groupsX, groupsY, 1U);

    // Effects pipeline
    auto pingBarrier = renderImages.ping.createWriteToRead();
    vk::DependencyInfo prepBarrier{};
    prepBarrier.setImageMemoryBarriers(pingBarrier);

    buffer->pipelineBarrier2(prepBarrier);

    auto* readImage = &renderImages.ping;
    auto* writeImage = &renderImages.pong;

    const auto* currentDescriptor = &renderDescriptors.computeAtoB;
    const auto* nextDescriptor = &renderDescriptors.computeBtoA;

    const auto* graphicsDescriptor = &renderDescriptors.graphicsA;
    const auto* graphicsNextDescriptor = &renderDescriptors.graphicsB;

    for (const auto& effect : mConfig.appData.effects) {
        if (!effect.enabled) continue;

        const auto& id = effect.effect->getId();
        const auto& pipeline = mConfig.pipelineSet.effectPipelines.at(id);

        buffer->bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.getVkHandle());

        auto computeDescSet = currentDescriptor->getVkHandle();
        vk::BindDescriptorSetsInfo computeBindInfo{};
        computeBindInfo.setStageFlags(vk::ShaderStageFlagBits::eCompute);
        computeBindInfo.setLayout(pipeline.getLayout());
        computeBindInfo.setDescriptorSets(computeDescSet);
        computeBindInfo.setFirstSet(0U);
        computeBindInfo.setDynamicOffsets(nullptr);
        buffer->bindDescriptorSets2(computeBindInfo);

        buffer->dispatch(groupsX, groupsY, 1U);

        auto readBarrier = readImage->createReadToWrite();
        auto writeBarrier = writeImage->createWriteToRead();
        std::array barriers{ readBarrier, writeBarrier };

        vk::DependencyInfo pingPongBarriers{};
        pingPongBarriers.setImageMemoryBarriers(barriers);

        buffer->pipelineBarrier2(pingPongBarriers);

        std::swap(readImage, writeImage);
        std::swap(currentDescriptor, nextDescriptor);
        std::swap(graphicsDescriptor, graphicsNextDescriptor);
    }

    readImage->transitionComputeToFragmentRead(buffer.get());

    // Graphics pipeline
    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(mConfig.renderpass.getVkHandle());
    renderPassInfo.setFramebuffer((*mConfig.framebuffers)[imageIndex].getVkHandle());
    renderPassInfo.renderArea.setOffset({ 0u, 0u });
    renderPassInfo.renderArea.setExtent(mConfig.extent);

    vk::ClearValue clearValue{ { 0.0f, 0.0f, 0.0f, 1.0f } };
    renderPassInfo.setClearValues(clearValue);

    buffer->beginRenderPass2(renderPassInfo, vk::SubpassContents::eInline);

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

    auto descriptorSet = graphicsDescriptor->getVkHandle();;
    vk::BindDescriptorSetsInfo graphicsBindInfo{};
    graphicsBindInfo.setStageFlags(vk::ShaderStageFlagBits::eAllGraphics);
    graphicsBindInfo.setLayout(mConfig.graphicsPipeline.getLayout());
    graphicsBindInfo.setDescriptorSets(descriptorSet);
    graphicsBindInfo.setFirstSet(0U);
    graphicsBindInfo.setDynamicOffsets(nullptr);
    buffer->bindDescriptorSets2(graphicsBindInfo);

    std::array pushValues = { mConfig.appData.mix };
    vk::PushConstantsInfo pushConstInfo{};
    pushConstInfo.setLayout(mConfig.graphicsPipeline.getLayout());
    pushConstInfo.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    pushConstInfo.setOffset(0U);
    pushConstInfo.setValues<float>(pushValues);
    buffer->pushConstants2(pushConstInfo);

    buffer->drawIndexed(mConfig.drawIndexCount, mConfig.drawInstanceCount, 0U, 0U, 0U);

    recordImGui(currentFrame, imageIndex);

    buffer->endRenderPass2(vk::SubpassEndInfo{});

    readImage->transitionRevertToCompute(buffer.get());

    if (readImage == &renderImages.ping)
    {
        auto revertReadBarrier = readImage->createReadToWrite();
        auto revertWriteBarrier = writeImage->createWriteToRead();
        std::array revertBarriers{ revertReadBarrier, revertWriteBarrier };

        vk::DependencyInfo revertBarrier{};
        revertBarrier.setImageMemoryBarriers(revertBarriers);
    
        buffer->pipelineBarrier2(revertBarrier);
    }

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
