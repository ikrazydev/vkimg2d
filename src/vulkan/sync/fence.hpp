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

    void reset() const;
    void wait(uint64_t timeout = UINT64_MAX) const;
private:
    const Device& mDevice;

    vk::UniqueFence mFence;
};

class BatchedFences
{
public:
    BatchedFences(const Device& device, const FenceConfig& config, uint32_t count);

    [[nodiscard]] const Fence& getFence(uint32_t index) const noexcept;
    [[nodiscard]] const vk::Fence getVkHandle(uint32_t index) const noexcept;
    [[nodiscard]] uint32_t getCount() const noexcept;
private:
    std::vector<Fence> mFences;
};
