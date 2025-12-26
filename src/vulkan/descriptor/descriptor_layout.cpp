#include "descriptor_layout.hpp"

#include <vulkan/device.hpp>

DescriptorLayout::DescriptorLayout(const Device& device, const DescriptorLayoutConfig& config)
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings(config.bindings.size());

    for (size_t i = 0; i < bindings.size(); i++) {
        const auto& configBinding = config.bindings.at(i);

        vk::DescriptorSetLayoutBinding binding{};
        binding.binding = configBinding.binding;
        binding.descriptorCount = 1U;
        binding.descriptorType = configBinding.type;
        binding.pImmutableSamplers = nullptr;
        binding.stageFlags = config.stages;

        bindings[i] = binding;
    }

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(bindings);

    mDescriptorLayout = device.getVkHandle().createDescriptorSetLayoutUnique(layoutInfo);
}

const vk::DescriptorSetLayout DescriptorLayout::getVkHandle() const
{
    return mDescriptorLayout.get();
}
