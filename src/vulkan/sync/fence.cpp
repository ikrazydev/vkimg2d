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

void Fence::reset() const
{
    mDevice.getVkHandle().resetFences(getVkHandle());
}

void Fence::wait(uint64_t timeout) const
{
    mDevice.getVkHandle().waitForFences(getVkHandle(), vk::True, timeout);
}

BatchedFences::BatchedFences(const Device& device, const FenceConfig& config, uint32_t count)
{
}

const Fence& BatchedFences::getFence(uint32_t index) const noexcept
{
    return mFences[index];
}

const vk::Fence BatchedFences::getVkHandle(uint32_t index) const
{
    return getFence(index).getVkHandle();
}

uint32_t BatchedFences::getCount() const
{
    return static_cast<uint32_t>(mFences.size());
}
