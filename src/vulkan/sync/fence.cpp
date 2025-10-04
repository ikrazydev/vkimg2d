#include "fence.hpp"

#include <vulkan/device.hpp>

Fence::Fence(const Device& device, const FenceConfig& config)
    : mDevice{ device }
{
    vk::FenceCreateInfo createInfo{};
    createInfo.setFlags(config.signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags{});

    mFence = mDevice.getVkHandle().createFenceUnique(createInfo);
}

const vk::Fence Fence::getVkHandle() const
{
    return mFence.get();
}

BatchedFences::BatchedFences(const Device& device, const FenceConfig& config, uint32_t count)
{
}

const vk::Fence BatchedFences::getVkHandle(uint32_t index) const
{
    return mFences[index].getVkHandle();
}

uint32_t BatchedFences::getCount() const
{
    return static_cast<uint32_t>(mFences.size());
}
