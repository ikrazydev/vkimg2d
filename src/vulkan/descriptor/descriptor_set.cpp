#include "descriptor_set.hpp"

#include <vulkan/device.hpp>

DescriptorSet::DescriptorSet(const Device& device, const DescriptorSetConfig& config)
    : mDevice{ device }
{
    std::vector layouts(config.setCount, config.descriptorLayout.getVkHandle());

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = config.descriptorPool.getVkHandle();
    allocInfo.setSetLayouts(layouts);

    mSets = device.getVkHandle().allocateDescriptorSetsUnique(allocInfo);
}

void DescriptorSet::update(const DescriptorUpdateConfig& config) const
{
    for (size_t set = 0; set < mSets.size(); set++) {
        std::vector<vk::WriteDescriptorSet> descriptorWrites;
        std::vector<vk::DescriptorImageInfo> imageInfos;
        descriptorWrites.reserve(config.images.size());
        imageInfos.reserve(config.images.size());

        for (size_t i = 0; i < config.images.size(); i++) {
            const auto& image = config.images[i];

            vk::DescriptorImageInfo imageInfo{};
            imageInfo.setImageLayout(image.layout);
            imageInfo.setImageView(image.texture.getImageView());
            if (image.sampler != nullptr)
                imageInfo.setSampler(image.sampler->getVkHandle());

            imageInfos.push_back(imageInfo);

            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite.setDstSet(mSets[set].get());
            descriptorWrite.setDstBinding(image.binding);
            descriptorWrite.setDstArrayElement(0U);
            descriptorWrite.setDescriptorType(image.descriptorType);
            descriptorWrite.setDescriptorCount(1U);
            descriptorWrite.setPImageInfo(&imageInfos.back());

            descriptorWrites.push_back(descriptorWrite);
        }

        mDevice.getVkHandle().updateDescriptorSets(descriptorWrites, nullptr);
    }
}

vk::DescriptorSet DescriptorSet::getVkHandle(size_t index) const noexcept
{
    return mSets[index].get();
}
