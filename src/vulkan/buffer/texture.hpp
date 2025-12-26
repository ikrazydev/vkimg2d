#pragma once

#include <optional>

#include <vulkan/include.hpp>
#include <vulkan/buffer/commandpool.hpp>

#include <io/image.hpp>

class Device;

enum class TextureImageType
{
    Sampled,
    Compute,
    SampledCompute,
};

struct TextureImageConfig
{
    const CommandPool& commandPool;
    const ImageLoadResult& image;

    TextureImageType type;
};

struct ComputeImageConfig
{
    const CommandPool& commandPool;

    uint32_t width, height;
};

class TextureImageView
{
public:
    TextureImageView(const Device& device, const vk::Image image, vk::Format format);

    const vk::ImageView getVkHandle() const;
private:
    vk::UniqueImageView mView;
};

class TextureImage
{
public:
    TextureImage(const Device& device, const TextureImageConfig& config);
    TextureImage(const Device& device, const ComputeImageConfig& config);

    const vk::Image getVkHandle() const;
    const vk::DeviceMemory getMemory() const;

    const vk::ImageView getImageView() const;

    const Device& getDevice() const;
private:
    void _transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    const Device& mDevice;
    const CommandPool& mCommandPool;

    vk::UniqueImage mImage;
    vk::UniqueDeviceMemory mMemory;

    std::optional<TextureImageView> mImageView;
};
