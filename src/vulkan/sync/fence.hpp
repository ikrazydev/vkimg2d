#pragma once

#include <vector>

#include <vulkan/include.hpp>

class Device;

struct FenceConfig
{
    bool signaled;
};

class Fence
{
public:
    Fence(const Device& device, const FenceConfig& config);

    [[nodiscard]] const vk::Fence getVkHandle() const noexcept;
private:
    const Device& mDevice;

    vk::UniqueFence mFence;
};

class BatchedFences
{
public:
    BatchedFences(const Device& device, const FenceConfig& config, uint32_t count);

    [[nodiscard]] const vk::Fence getVkHandle(uint32_t index) const noexcept;
    [[nodiscard]] uint32_t getCount() const noexcept;
private:
    std::vector<Fence> mFences;
};
