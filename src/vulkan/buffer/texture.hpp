#pragma once

#include <optional>

#include <io/image.hpp>

#include <vulkan/include.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/commandpool.hpp>

class Device;

class TextureImage
{
public:
    TextureImage(const Device& device, const CommandPool& commandPool, const ImageLoadResult& image);

    const vk::Image getVkHandle() const;
    const vk::DeviceMemory getMemory() const;

    const Device& getDevice() const;
private:
    void transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    const Device& mDevice;
    const CommandPool& mCommandPool;

    vk::UniqueImage mImage;
    vk::UniqueDeviceMemory mMemory;

    std::optional<TextureImageView> mImageView;
};

class TextureImageView
{
public:
    TextureImageView(const TextureImage& image);

    const vk::ImageView getVkHandle() const;
private:
    vk::UniqueImageView mView;
};
