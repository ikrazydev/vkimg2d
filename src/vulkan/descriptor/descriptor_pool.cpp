#include "descriptor_pool.hpp"

#include <vulkan/device.hpp>

DescriptorPool::DescriptorPool(const Device& device, const DescriptorPoolConfig& config)
{
    std::vector<vk::DescriptorPoolSize> poolSizes;
    poolSizes.reserve(config.sizes.size());

    for (const auto& configSize : config.sizes) {
        vk::DescriptorPoolSize poolSize{};
        poolSize.setType(configSize.type);
        poolSize.setDescriptorCount(configSize.count);

        poolSizes.push_back(poolSize);
    }

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.setPoolSizes(poolSizes);
    poolInfo.setMaxSets(config.maxSets);
    poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    mPool = device.getVkHandle().createDescriptorPoolUnique(poolInfo);
}

const vk::DescriptorPool DescriptorPool::getVkHandle() const
{
    return mPool.get();
}
