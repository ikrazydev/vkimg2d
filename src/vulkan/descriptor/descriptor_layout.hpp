#pragma once

#include <vector>

#include <vulkan/include.hpp>

class Device;

struct DescriptorLayoutBindingConfig
{
    uint32_t binding;
    vk::DescriptorType type;
};

struct DescriptorLayoutConfig
{
    const std::vector<DescriptorLayoutBindingConfig>& bindings;
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
