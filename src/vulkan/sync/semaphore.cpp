#include "semaphore.hpp"

#include <vulkan/device.hpp>

static constexpr vk::SemaphoreCreateInfo gSemaphoreInfo{};

Semaphore::Semaphore(const Device& device)
    : mDevice{ device }
{
    mSemaphore = mDevice.getVkHandle().createSemaphoreUnique(gSemaphoreInfo);
}

const vk::Semaphore Semaphore::getVkHandle() const
{
    return mSemaphore.get();
}

BatchedSemaphores::BatchedSemaphores(const Device& device, size_t count)
{
    mSemaphores.reserve(count);

    std::generate_n(std::back_inserter(mSemaphores), count, [&device]() { return Semaphore{ device }; });
}

const vk::Semaphore BatchedSemaphores::getVkHandle(size_t index) const noexcept
{
    return mSemaphores[index].getVkHandle();
}

uint32_t BatchedSemaphores::getCount() const noexcept
{
    return static_cast<uint32_t>(mSemaphores.size());
}
