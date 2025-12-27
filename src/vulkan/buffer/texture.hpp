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

    TextureImage(TextureImage&&) = default;
    TextureImage& operator=(TextureImage&&) = default;

    TextureImage(const TextureImage&) = delete;
    TextureImage& operator=(const TextureImage&) = delete;

    [[nodiscard]] vk::Image getVkHandle() const noexcept;
    [[nodiscard]] vk::DeviceMemory getMemory() const noexcept;

    [[nodiscard]] vk::Extent2D getExtent() const noexcept;
    [[nodiscard]] uint32_t getWidth() const noexcept;
    [[nodiscard]] uint32_t getHeight() const noexcept;

    [[nodiscard]] vk::Format getFormat() const noexcept;

    [[nodiscard]] vk::ImageView getImageView() const noexcept;

    [[nodiscard]] const Device& getDevice() const noexcept;

    vk::ImageMemoryBarrier2 createReadToWrite() const;
    vk::ImageMemoryBarrier2 createWriteToRead() const;

    bool isComputeFrameReady() const;

    void transitionComputeToFragmentRead(vk::CommandBuffer buffer);
    void transitionRevertToCompute(vk::CommandBuffer buffer);
private:
    vk::ImageMemoryBarrier2 _prepareBarrier(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
    void _commitBarrier(vk::CommandBuffer buffer, vk::ImageMemoryBarrier2 barrier) const;
    void _transitionImageLayout(vk::CommandBuffer buffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;
    void _transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;

    const Device& mDevice;
    const CommandPool& mCommandPool;

    vk::Extent2D mExtent;
    vk::Format mFormat;

    vk::UniqueImage mImage;
    vk::UniqueDeviceMemory mMemory;

    std::optional<TextureImageView> mImageView;

    bool mComputeFrameReady = false; // TODO: maybe it won't be needed; or implement better
};
