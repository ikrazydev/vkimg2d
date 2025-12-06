#include "descriptor_layout.hpp"

#include <vulkan/device.hpp>

DescriptorLayout::DescriptorLayout(const Device& device, const DescriptorLayoutConfig& config)
{
    vk::DescriptorSetLayoutBinding samplerBinding{};
    samplerBinding.binding = config.binding;
    samplerBinding.descriptorCount = 1U;
    samplerBinding.descriptorType = config.type;
    samplerBinding.pImmutableSamplers = nullptr;
    samplerBinding.stageFlags = config.stages;

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = 1U;
    layoutInfo.pBindings = &samplerBinding;

    mDescriptorLayout = device.getVkHandle().createDescriptorSetLayoutUnique(layoutInfo);
}

const vk::DescriptorSetLayout DescriptorLayout::getVkHandle() const
{
    return mDescriptorLayout.get();
}
