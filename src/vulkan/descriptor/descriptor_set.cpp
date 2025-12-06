#include "descriptor_set.hpp"

#include <vulkan/device.hpp>

DescriptorSet::DescriptorSet(const Device& device, const DescriptorSetConfig& config)
{
    std::vector layouts(config.count, config.descriptorLayout.getVkHandle());

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = config.descriptorPool.getVkHandle();
    allocInfo.setSetLayouts(layouts);

    mDescriptorSets.resize(allocInfo.descriptorSetCount);

    if (vkAllocateDescriptorSets(mDevice, &allocInfo, mDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets.");
    }

    for (size_t i = 0; i < config.count; i++) {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfo.imageView = config.texture.getView();
        imageInfo.sampler = config.sampler.getVkHandle();

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = mDescriptorSets[i];
        descriptorWrite.dstBinding = 0U;
        descriptorWrite.dstArrayElement = 0U;
        descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrite.descriptorCount = 1U;
        descriptorWrite.pImageInfo = &imageInfo;

        device.getVkHandle().updateDescriptorSets(descriptorWrite, nullptr);
    }
}
