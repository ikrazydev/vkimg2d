#include "descriptor_set.hpp"

#include <utility>

#include <vulkan/device.hpp>

DescriptorSet::DescriptorSet(const Device& device, const DescriptorSetConfig& config)
    : mDevice{ device }
{
    auto layout = config.descriptorLayout.getVkHandle();

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.setDescriptorPool(config.descriptorPool.getVkHandle());
    allocInfo.setSetLayouts(layout);

    auto sets = device.getVkHandle().allocateDescriptorSetsUnique(allocInfo);
    mSet = std::move(sets.at(0U));
}

void DescriptorSet::update(const DescriptorUpdateConfig& config) const
{
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
        descriptorWrite.setDstSet(mSet.get());
        descriptorWrite.setDstBinding(image.binding);
        descriptorWrite.setDstArrayElement(0U);
        descriptorWrite.setDescriptorType(image.descriptorType);
        descriptorWrite.setDescriptorCount(1U);
        descriptorWrite.setPImageInfo(&imageInfos.back());

        descriptorWrites.push_back(descriptorWrite);
    }

    mDevice.getVkHandle().updateDescriptorSets(descriptorWrites, nullptr);
}

vk::DescriptorSet DescriptorSet::getVkHandle() const noexcept
{
    return mSet.get();
}
