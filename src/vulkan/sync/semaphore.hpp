#pragma once

#include <vulkan/include.hpp>

#include <vector>

class Device;

class Semaphore
{
public:
    Semaphore(const Device& device);

    [[nodiscard]] const vk::Semaphore getVkHandle() const;
private:
    const Device& mDevice;

    vk::UniqueSemaphore mSemaphore;
};

class BatchedSemaphores
{
public:
    BatchedSemaphores(const Device& device, size_t count);

    [[nodiscard]] const vk::Semaphore getVkHandle(size_t index) const noexcept;
    [[nodiscard]] uint32_t getCount() const noexcept;
private:
    std::vector<Semaphore> mSemaphores;
};
