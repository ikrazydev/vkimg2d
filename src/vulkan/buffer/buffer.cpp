#include "buffer.hpp"

#include <vulkan/device.hpp>
#include <vulkan/buffer/commandbuffer.hpp>

Buffer::Buffer(const Device& device, const BufferConfig& config)
    : mDevice{ device }
    , mCommandPool{ config.commandPool }
{
    const auto deviceHandle = device.getVkHandle();
    const auto physicalDevice = device.getPhysicalDevice();

    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = config.size;
    bufferInfo.usage = config.usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    mBuffer = deviceHandle.createBufferUnique(bufferInfo);

    vk::MemoryRequirements memoryRequirements = deviceHandle.getBufferMemoryRequirements(mBuffer.get());

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, config.properties);

    mMemory = deviceHandle.allocateMemoryUnique(allocInfo);
    deviceHandle.bindBufferMemory(mBuffer.get(), mMemory.get(), 0U);
}

void Buffer::copy(const Buffer& dstBuffer, vk::DeviceSize size) const
{
    auto commandBuffer = SingleTimeCommandBuffer{ mDevice, mCommandPool };

    vk::BufferCopy region{};
    region.srcOffset = 0U;
    region.dstOffset = 0U;
    region.size = size;
    
    commandBuffer.getVkHandle().copyBuffer(mBuffer.get(), dstBuffer.getVkHandle(), region);
}

void Buffer::copyToImage(vk::Image image, uint32_t width, uint32_t height) const
{
    auto commandBuffer = SingleTimeCommandBuffer{ mDevice, mCommandPool };

    vk::BufferImageCopy region{};
    region.bufferOffset = 0U;
    region.bufferRowLength = 0U;
    region.bufferImageHeight = 0U;

    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0U;
    region.imageSubresource.baseArrayLayer = 0U;
    region.imageSubresource.layerCount = 1U;

    region.imageOffset = vk::Offset3D{ 0, 0, 0 };
    region.imageExtent = vk::Extent3D{ width, height, 1U };

    commandBuffer.getVkHandle().copyBufferToImage(mBuffer.get(), image, vk::ImageLayout::eTransferDstOptimal, region);
}

const vk::Buffer Buffer::getVkHandle() const
{
    return mBuffer.get();
}

const vk::DeviceMemory Buffer::getMemory() const
{
    return mMemory.get();
}

uint32_t Buffer::findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    auto memProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i)
            && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

Buffer Buffer::createTransitioned(const Device& device, const TransitionedBufferConfig& config)
{
    BufferConfig stagingBufferConfig = {
        .size = config.size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,

        .commandPool = config.commandPool,
    };

    Buffer stagingBuffer{ device, stagingBufferConfig };

    void* data = device.getVkHandle().mapMemory(stagingBuffer.getMemory(), 0U, config.size, vk::MemoryMapFlags());
    memcpy(data, config.data, static_cast<size_t>(config.size));
    device.getVkHandle().unmapMemory(stagingBuffer.getMemory());

    BufferConfig bufferConfig = {
        .size = config.size,
        .usage = vk::BufferUsageFlagBits::eTransferDst | config.usage,
        .properties = vk::MemoryPropertyFlagBits::eDeviceLocal,

        .commandPool = config.commandPool,
    };

    Buffer buffer{ device, bufferConfig };

    stagingBuffer.copy(buffer, config.size);

    return buffer;
}

Buffer Buffer::createVertex(const Device& device, const CommandPool& commandPool, const std::vector<Vertex>& vertices)
{
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    TransitionedBufferConfig config = {
        .commandPool = commandPool,

        .size = bufferSize,
        .data = vertices.data(),

        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
    };

    return createTransitioned(device, config);
}

Buffer Buffer::createIndex(const Device& device, const CommandPool& commandPool, const std::vector<uint32_t>& indices)
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    TransitionedBufferConfig config = {
        .commandPool = commandPool,

        .size = bufferSize,
        .data = indices.data(),

        .usage = vk::BufferUsageFlagBits::eIndexBuffer,
    };

    return createTransitioned(device, config);
}
