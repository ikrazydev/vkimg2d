#pragma once

#include <vulkan/include.hpp>

class Device;

struct CommandPoolConfig
{
    uint32_t queueFamilyIndex;
};

class CommandPool
{
public:
    CommandPool(const Device& device, const CommandPoolConfig& config);

    const vk::CommandPool& getVkHandle() const;
private:
    vk::UniqueCommandPool mPool;
};
