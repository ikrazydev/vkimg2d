#pragma once

#include <vulkan/include.hpp>
#include <vulkan/buffer/commandpool.hpp>

struct BufferConfig
{
    vk::DeviceSize size;
    vk::BufferUsageFlags usage;
    vk::MemoryPropertyFlags properties;

    const CommandPool& commandPool;
};

class Device;

class Buffer
{
public:
    Buffer(const Device& device, const BufferConfig& config);

    void copy(const Buffer& dstBuffer, vk::DeviceSize size) const;
    void copyToImage(vk::Image image, uint32_t width, uint32_t height) const;

    const vk::Buffer getVkHandle() const;
    const vk::DeviceMemory getMemory() const;

    static uint32_t findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);
private:
    const Device& mDevice;
    const CommandPool& mCommandPool;

    vk::UniqueBuffer mBuffer;
    vk::UniqueDeviceMemory mMemory;
};
