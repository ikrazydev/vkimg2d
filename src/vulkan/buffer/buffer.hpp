#pragma once

#include <vulkan/include.hpp>
#include <vulkan/vertex.hpp>
#include <vulkan/buffer/commandpool.hpp>

struct BufferConfig
{
    vk::DeviceSize size;
    vk::BufferUsageFlags usage;
    vk::MemoryPropertyFlags properties;

    const CommandPool& commandPool;
};

struct TransitionedBufferConfig
{
    const CommandPool& commandPool;

    vk::DeviceSize size;
    const void* data;

    vk::BufferUsageFlagBits usage;
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

    static Buffer createTransitioned(const Device& device, const TransitionedBufferConfig& config);
    static Buffer createVertex(const Device& device, const CommandPool& commandPool, const std::vector<Vertex>& vertices);
    static Buffer createIndex(const Device& device, const CommandPool& commandPool, const std::vector<uint32_t>& indices);
private:
    const Device& mDevice;
    const CommandPool& mCommandPool;

    vk::UniqueBuffer mBuffer;
    vk::UniqueDeviceMemory mMemory;
};
