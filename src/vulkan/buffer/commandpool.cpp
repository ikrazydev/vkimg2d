#include "commandpool.hpp"

#include <vulkan/device.hpp>

CommandPool::CommandPool(const Device& device, const CommandPoolConfig& config)
{
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    poolInfo.setQueueFamilyIndex(config.queueFamilyIndex);

    mPool = device.getVkHandle().createCommandPoolUnique(poolInfo);
}

const vk::CommandPool& CommandPool::getVkHandle() const
{
    return mPool.get();
}
