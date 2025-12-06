#include "descriptor_pool.hpp"

#include <vulkan/device.hpp>

DescriptorPool::DescriptorPool(const Device& device, const DescriptorPoolConfig& config)
{
    vk::DescriptorPoolSize poolSize{};
    poolSize.type = config.type;
    poolSize.descriptorCount = config.count;

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = 1U;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = config.maxSets;
    poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

    mPool = device.getVkHandle().createDescriptorPoolUnique(poolInfo);
}

const vk::DescriptorPool DescriptorPool::getVkHandle() const
{
    return mPool.get();
}
