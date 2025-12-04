#pragma once

#include <io/image.hpp>

#include <vulkan/include.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/commandpool.hpp>

class Device;

class VkTextureImage
{
public:
    VkTextureImage(const Device& device, const CommandPool& commandPool, const ImageLoadResult& image);
private:
    void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    const Device& mDevice;
    const CommandPool& mCommandPool;

    vk::UniqueImage mImage;
    vk::UniqueDeviceMemory mMemory;
};
