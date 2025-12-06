#include "renderpass.hpp"

#include <vulkan/device.hpp>

Renderpass::Renderpass(const Device& device, const RenderpassConfig& config)
    : mDevice(device)
{
    vk::AttachmentDescription colorAttachment{};
    colorAttachment.setFormat(config.format);
    colorAttachment.setSamples(vk::SampleCountFlagBits::e1);

    colorAttachment.setLoadOp(config.colorLoadOp);
    colorAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    colorAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colorAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

    colorAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    colorAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorAttachmentRef{};
    colorAttachmentRef.setAttachment(0U);
    colorAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachments(colorAttachmentRef);

    vk::SubpassDependency dependency{};
    dependency.setSrcSubpass(vk::SubpassExternal);
    dependency.setDstSubpass(0U);
    dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setSrcAccessMask(vk::AccessFlags());
    dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.setAttachments(colorAttachment);
    renderPassInfo.setSubpasses(subpass);
    renderPassInfo.setDependencies(dependency);

    mRenderpass = device.getVkHandle().createRenderPassUnique(renderPassInfo);
}

const vk::RenderPass Renderpass::getVkHandle() const
{
    return mRenderpass.get();
}

const Device& Renderpass::getDevice() const
{
    return mDevice;
}
