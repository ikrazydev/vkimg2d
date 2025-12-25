#include "texture.hpp"

#include <vulkan/device.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/commandbuffer.hpp>

vk::ImageUsageFlags _imageTypeToFlags(TextureImageType type)
{
    auto flags = vk::ImageUsageFlags();

    if (type == TextureImageType::Sampled || type == TextureImageType::SampledCompute)
        flags |= vk::ImageUsageFlagBits::eSampled;
    if (type == TextureImageType::Compute || type == TextureImageType::SampledCompute)
        flags |= vk::ImageUsageFlagBits::eStorage;

    return flags;
}

vk::Format _imageTypeToFormat(TextureImageType type)
{
    if (type == TextureImageType::Compute || type == TextureImageType::SampledCompute)
        return vk::Format::eR8G8B8A8Unorm;

    return vk::Format::eR8G8B8A8Srgb;
}

TextureImage::TextureImage(const Device& device, const TextureImageConfig& config)
    : mDevice{ device }
    , mCommandPool{ config.commandPool }
{
    const auto& image = config.image;

    const auto deviceHandle = device.getVkHandle();
    vk::DeviceSize imageSize = image.texWidth * image.texHeight * 4;

    BufferConfig stagingBufferConfig = {
        .size = imageSize,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,

        .commandPool = config.commandPool,
    };

    Buffer stagingBuffer{ device, stagingBufferConfig };

    void* data = deviceHandle.mapMemory(stagingBuffer.getMemory(), 0U, imageSize, vk::MemoryMapFlags());
    memcpy(data, image.pixels, static_cast<size_t>(imageSize));
    deviceHandle.unmapMemory(stagingBuffer.getMemory());

    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D);
    imageInfo.extent.setWidth(static_cast<uint32_t>(image.texWidth));
    imageInfo.extent.setHeight(static_cast<uint32_t>(image.texHeight));
    imageInfo.extent.setDepth(1U);
    imageInfo.setMipLevels(1U);
    imageInfo.setArrayLayers(1U);
    imageInfo.setFormat(_imageTypeToFormat(config.type));
    imageInfo.setTiling(vk::ImageTiling::eOptimal);
    imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferDst | _imageTypeToFlags(config.type));
    imageInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageInfo.setFlags(vk::ImageCreateFlags());

    mImage = deviceHandle.createImageUnique(imageInfo);

    auto memoryRequirements = deviceHandle.getImageMemoryRequirements(mImage.get());

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memoryRequirements.size);
    allocInfo.setMemoryTypeIndex(Buffer::findMemoryType(device.getPhysicalDevice(), memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    mMemory = deviceHandle.allocateMemoryUnique(allocInfo);
    deviceHandle.bindImageMemory(mImage.get(), mMemory.get(), 0U);

    _transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    stagingBuffer.copyToImage(mImage.get(), static_cast<uint32_t>(image.texWidth), static_cast<uint32_t>(image.texHeight));
    _transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    mImageView.emplace(mDevice, mImage.get(), config);
}

const vk::Image TextureImage::getVkHandle() const
{
    return mImage.get();
}

const vk::DeviceMemory TextureImage::getMemory() const
{
    return mMemory.get();
}

const vk::ImageView TextureImage::getImageView() const
{
    return mImageView.value().getVkHandle();
}

const Device& TextureImage::getDevice() const
{
    return mDevice;
}

void TextureImage::_transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto commandBuffer = SingleTimeCommandBuffer{ mDevice, mCommandPool };

    vk::ImageMemoryBarrier barrier{};
    barrier.setOldLayout(oldLayout);
    barrier.setNewLayout(newLayout);
    barrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
    barrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);

    barrier.setImage(mImage.get());
    barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.subresourceRange.setBaseMipLevel(0U);
    barrier.subresourceRange.setBaseArrayLayer(0U);
    barrier.subresourceRange.setLevelCount(1U);
    barrier.subresourceRange.setLayerCount(1U);

    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;

    if (
        oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal
        ) {
        barrier.setSrcAccessMask(vk::AccessFlags());
        barrier.setDstAccessMask(vk::AccessFlags());

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (
        oldLayout == vk::ImageLayout::eTransferDstOptimal &&
        newLayout == vk::ImageLayout::eShaderReadOnlyOptimal
        ) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::runtime_error("Unsupported layout transition.");
    }

    commandBuffer.getVkHandle().pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
}

TextureImageView::TextureImageView(const Device& device, const vk::Image image, const TextureImageConfig& config)
{
    vk::ImageViewCreateInfo createInfo{};
    createInfo.setImage(image);
    createInfo.setViewType(vk::ImageViewType::e2D);
    createInfo.setFormat(_imageTypeToFormat(config.type));

    createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
    createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
    createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
    createInfo.components.setA(vk::ComponentSwizzle::eIdentity);

    createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    createInfo.subresourceRange.setBaseMipLevel(0U);
    createInfo.subresourceRange.setBaseArrayLayer(0U);
    createInfo.subresourceRange.setLevelCount(1U);
    createInfo.subresourceRange.setLayerCount(1U);

    mView = device.getVkHandle().createImageViewUnique(createInfo);
}

const vk::ImageView TextureImageView::getVkHandle() const
{
    return mView.get();
}
