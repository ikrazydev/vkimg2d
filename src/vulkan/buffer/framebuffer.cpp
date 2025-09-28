#include "framebuffer.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>

Framebuffer::Framebuffer(const Renderpass& renderpass, const std::vector<vk::ImageView>& attachments, vk::Extent2D extent)
    : mDevice{ renderpass.getDevice() }
    , mRenderpass{ renderpass }
{
    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderpass.getVkHandle());
    framebufferInfo.setAttachments(attachments);
    framebufferInfo.setWidth(extent.width);
    framebufferInfo.setHeight(extent.height);
    framebufferInfo.setLayers(1);

    mFramebuffer = mDevice.getVkHandle().createFramebufferUnique(framebufferInfo);
}
