#include "descriptor_set.hpp"

#include <vulkan/device.hpp>

DescriptorSet::DescriptorSet(const Device& device, const DescriptorSetConfig& config)
{
    std::vector layouts(config.count, config.descriptorLayout.getVkHandle());

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = config.descriptorPool.getVkHandle();
    allocInfo.setSetLayouts(layouts);

    mSets = device.getVkHandle().allocateDescriptorSetsUnique(allocInfo);

    for (size_t i = 0; i < config.count; i++) {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfo.imageView = config.texture.getImageView();
        imageInfo.sampler = config.sampler.getVkHandle();

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = mSets[i].get();
        descriptorWrite.dstBinding = 0U;
        descriptorWrite.dstArrayElement = 0U;
        descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrite.descriptorCount = 1U;
        descriptorWrite.pImageInfo = &imageInfo;

        device.getVkHandle().updateDescriptorSets(descriptorWrite, nullptr);
    }
}

const vk::DescriptorSet DescriptorSet::getVkHandle(size_t index) const
{
    return mSets[index].get();
}
