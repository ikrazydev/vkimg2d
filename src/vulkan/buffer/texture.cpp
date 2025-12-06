#include "texture.hpp"

#include <vulkan/device.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/commandbuffer.hpp>

TextureImage::TextureImage(const Device& device, const CommandPool& commandPool, const ImageLoadResult& image)
    : mDevice{ device }
    , mCommandPool{ commandPool }
{
    const auto deviceHandle = device.getVkHandle();
    vk::DeviceSize imageSize = image.texWidth * image.texHeight * 4;

    BufferConfig stagingBufferConfig = {
        .size = imageSize,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,

        .commandPool = commandPool,
    };

    Buffer stagingBuffer{ device, stagingBufferConfig };

    void* data = deviceHandle.mapMemory(stagingBuffer.getMemory(), 0U, imageSize, vk::MemoryMapFlags());
    memcpy(data, image.pixels, static_cast<size_t>(imageSize));
    deviceHandle.unmapMemory(stagingBuffer.getMemory());

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = static_cast<uint32_t>(image.texWidth);
    imageInfo.extent.height = static_cast<uint32_t>(image.texHeight);
    imageInfo.extent.depth = 1U;
    imageInfo.mipLevels = 1U;
    imageInfo.arrayLayers = 1U;
    imageInfo.format = vk::Format::eR8G8B8A8Srgb;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.flags = vk::ImageCreateFlags();

    mImage = deviceHandle.createImageUnique(imageInfo);

    auto memoryRequirements = deviceHandle.getImageMemoryRequirements(mImage.get());

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = Buffer::findMemoryType(device.getPhysicalDevice(), memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

    mMemory = deviceHandle.allocateMemoryUnique(allocInfo);
    deviceHandle.bindImageMemory(mImage.get(), mMemory.get(), 0U);

    transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    stagingBuffer.copyToImage(mImage.get(), static_cast<uint32_t>(image.texWidth), static_cast<uint32_t>(image.texHeight));
    transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    mImageView.emplace(mDevice, mImage.get());
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

void TextureImage::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto commandBuffer = SingleTimeCommandBuffer{ mDevice, mCommandPool };

    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
    barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;

    barrier.image = mImage.get();
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0U;
    barrier.subresourceRange.levelCount = 1U;
    barrier.subresourceRange.baseArrayLayer = 0U;
    barrier.subresourceRange.layerCount = 1U;

    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;

    if (
        oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal
        ) {
        barrier.srcAccessMask = vk::AccessFlags();
        barrier.dstAccessMask = vk::AccessFlags();

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (
        oldLayout == vk::ImageLayout::eTransferDstOptimal &&
        newLayout == vk::ImageLayout::eShaderReadOnlyOptimal
        ) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else {
        throw std::runtime_error("Unsupported layout transition.");
    }

    commandBuffer.getVkHandle().pipelineBarrier(srcStage, dstStage, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);
}

TextureImageView::TextureImageView(const Device& device, const vk::Image image)
{
    vk::ImageViewCreateInfo createInfo{};
    createInfo.image = image;
    createInfo.viewType = vk::ImageViewType::e2D;
    createInfo.format = vk::Format::eR8G8B8A8Srgb;

    createInfo.components.r = vk::ComponentSwizzle::eIdentity;
    createInfo.components.g = vk::ComponentSwizzle::eIdentity;
    createInfo.components.b = vk::ComponentSwizzle::eIdentity;
    createInfo.components.a = vk::ComponentSwizzle::eIdentity;

    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0U;
    createInfo.subresourceRange.baseArrayLayer = 0U;
    createInfo.subresourceRange.levelCount = 1U;
    createInfo.subresourceRange.layerCount = 1U;

    mView = device.getVkHandle().createImageViewUnique(createInfo);
}

const vk::ImageView TextureImageView::getVkHandle() const
{
    return mView.get();
}
