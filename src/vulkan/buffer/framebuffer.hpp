#pragma once

#include <vulkan/include.hpp>

#include <vector>

class Device;
class Renderpass;

class Framebuffer
{
public:
    Framebuffer(const Renderpass& renderpass, const std::vector<vk::ImageView>& attachments, vk::Extent2D extent);
private:
    const Device& mDevice;
    const Renderpass& mRenderpass;

    vk::UniqueFramebuffer mFramebuffer;
};
