#pragma once

#include <vulkan/include.hpp>

class Device;

struct RenderpassConfig
{
    vk::Format format;

    vk::AttachmentLoadOp colorLoadOp;
};

class Renderpass
{
public:
    Renderpass(const Device& device, const RenderpassConfig& config);

    const vk::RenderPass getVkHandle() const;
    const Device& getDevice() const;
private:
    const Device& mDevice;

    vk::UniqueRenderPass mRenderpass;
};
