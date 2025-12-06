#pragma once

#include <vulkan/include.hpp>

class Device;

struct DescriptorLayoutConfig
{
    uint32_t binding;
    vk::DescriptorType type;
    vk::ShaderStageFlagBits stages;
};

class DescriptorLayout
{
public:
    DescriptorLayout(const Device& device, const DescriptorLayoutConfig& config);

    const vk::DescriptorSetLayout getVkHandle() const;
private:
    vk::UniqueDescriptorSetLayout mDescriptorLayout;
};
