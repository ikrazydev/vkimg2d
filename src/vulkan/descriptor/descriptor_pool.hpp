#pragma once

#include <vulkan/include.hpp>

struct DescriptorPoolConfig
{
    vk::DescriptorType type;
    uint32_t count;
    uint32_t maxSets;
};

class Device;

class DescriptorPool
{
public:
    DescriptorPool(const Device& device, const DescriptorPoolConfig& config);

    const vk::DescriptorPool getVkHandle() const;
private:
    vk::UniqueDescriptorPool mPool;
};
