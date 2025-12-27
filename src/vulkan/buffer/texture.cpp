#include "texture.hpp"

#include <vulkan/device.hpp>
#include <vulkan/buffer/buffer.hpp>
#include <vulkan/buffer/commandbuffer.hpp>

static vk::ImageUsageFlags _imageTypeToFlags(TextureImageType type)
{
    auto flags = vk::ImageUsageFlags();

    if (type == TextureImageType::Sampled || type == TextureImageType::SampledCompute)
        flags |= vk::ImageUsageFlagBits::eSampled;
    if (type == TextureImageType::Compute || type == TextureImageType::SampledCompute)
        flags |= vk::ImageUsageFlagBits::eStorage;

    return flags;
}

static vk::Format _imageTypeToFormat(TextureImageType type)
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

    mFormat = imageInfo.format;
    mExtent = vk::Extent2D{ imageInfo.extent.width, imageInfo.extent.height };

    auto memoryRequirements = deviceHandle.getImageMemoryRequirements(mImage.get());

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memoryRequirements.size);
    allocInfo.setMemoryTypeIndex(Buffer::findMemoryType(device.getPhysicalDevice(), memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    mMemory = deviceHandle.allocateMemoryUnique(allocInfo);
    deviceHandle.bindImageMemory(mImage.get(), mMemory.get(), 0U);

    _transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    stagingBuffer.copyToImage(mImage.get(), static_cast<uint32_t>(image.texWidth), static_cast<uint32_t>(image.texHeight));
    _transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    mImageView.emplace(mDevice, mImage.get(), imageInfo.format);
}

TextureImage::TextureImage(const Device& device, const ComputeImageConfig& config)
    : mDevice{ device }
    , mCommandPool{ config.commandPool }
{
    const auto deviceHandle = device.getVkHandle();
    auto imageType = TextureImageType::SampledCompute;

    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D);
    imageInfo.extent.setWidth(config.width);
    imageInfo.extent.setHeight(config.height);
    imageInfo.extent.setDepth(1U);
    imageInfo.setMipLevels(1U);
    imageInfo.setArrayLayers(1U);
    imageInfo.setFormat(_imageTypeToFormat(imageType));
    imageInfo.setTiling(vk::ImageTiling::eOptimal);
    imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageInfo.setUsage(_imageTypeToFlags(imageType));
    imageInfo.setSharingMode(vk::SharingMode::eExclusive);
    imageInfo.setSamples(vk::SampleCountFlagBits::e1);
    imageInfo.setFlags(vk::ImageCreateFlags());

    mImage = deviceHandle.createImageUnique(imageInfo);

    mFormat = imageInfo.format;
    mExtent = vk::Extent2D{ imageInfo.extent.width, imageInfo.extent.height };

    auto memoryRequirements = deviceHandle.getImageMemoryRequirements(mImage.get());

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memoryRequirements.size);
    allocInfo.setMemoryTypeIndex(Buffer::findMemoryType(device.getPhysicalDevice(), memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    mMemory = deviceHandle.allocateMemoryUnique(allocInfo);
    deviceHandle.bindImageMemory(mImage.get(), mMemory.get(), 0U);

    _transitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
    mComputeFrameReady = true;

    mImageView.emplace(device, mImage.get(), imageInfo.format);
}

vk::Image TextureImage::getVkHandle() const noexcept
{
    return mImage.get();
}

vk::DeviceMemory TextureImage::getMemory() const noexcept
{
    return mMemory.get();
}

vk::Extent2D TextureImage::getExtent() const noexcept
{
    return mExtent;
}

uint32_t TextureImage::getWidth() const noexcept
{
    return getExtent().width;
}

uint32_t TextureImage::getHeight() const noexcept
{
    return getExtent().height;
}

vk::Format TextureImage::getFormat() const noexcept
{
    return mFormat;
}

vk::ImageView TextureImage::getImageView() const noexcept
{
    return mImageView.value().getVkHandle();
}

const Device& TextureImage::getDevice() const noexcept
{
    return mDevice;
}

vk::ImageMemoryBarrier2 TextureImage::_prepareBarrier(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const
{
    vk::ImageMemoryBarrier2 barrier{};
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

    return barrier;
}

void TextureImage::_commitBarrier(vk::CommandBuffer buffer, vk::ImageMemoryBarrier2 barrier) const
{
    vk::DependencyInfo depInfo{};
    depInfo.setImageMemoryBarriers(barrier);

    buffer.pipelineBarrier2(depInfo);
}

void TextureImage::_transitionImageLayout(vk::CommandBuffer buffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const
{
    auto barrier = _prepareBarrier(oldLayout, newLayout);

    if (
        oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal
        ) {
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eTransferWrite);

        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eTransfer);
    }
    else if (
        oldLayout == vk::ImageLayout::eTransferDstOptimal &&
        newLayout == vk::ImageLayout::eShaderReadOnlyOptimal
        ) {
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderRead);

        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader);
    }
    else if (
        oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eGeneral
        ) {
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eNone);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderWrite);

        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader);
    }
    else if (
        oldLayout == vk::ImageLayout::eGeneral &&
        newLayout == vk::ImageLayout::eShaderReadOnlyOptimal
        ) {
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderSampledRead);

        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eFragmentShader);
    }
    else if (
        oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal &&
        newLayout == vk::ImageLayout::eGeneral
        ) {
        barrier.setSrcAccessMask(vk::AccessFlagBits2::eShaderSampledRead);
        barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderStorageWrite);
        barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eFragmentShader);
        barrier.setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader);
    }
    else {
        throw std::runtime_error("Unsupported layout transition.");
    }

    _commitBarrier(buffer, barrier);
}

void TextureImage::_transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const
{
    auto commandBuffer = SingleTimeCommandBuffer{ mDevice, mCommandPool };

    _transitionImageLayout(commandBuffer.getVkHandle(), oldLayout, newLayout);
}

vk::ImageMemoryBarrier2 TextureImage::createReadToWrite() const
{
    auto barrier = _prepareBarrier(vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral);
    barrier.setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageRead);
    barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderStorageWrite);
    barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader);
    barrier.setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader);

    return barrier;
}

vk::ImageMemoryBarrier2 TextureImage::createWriteToRead() const
{
    auto barrier = _prepareBarrier(vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral);
    barrier.setSrcAccessMask(vk::AccessFlagBits2::eShaderStorageWrite);
    barrier.setDstAccessMask(vk::AccessFlagBits2::eShaderStorageRead);
    barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eComputeShader);
    barrier.setDstStageMask(vk::PipelineStageFlagBits2::eComputeShader);

    return barrier;
}

bool TextureImage::isComputeFrameReady() const
{
    return mComputeFrameReady;
}

void TextureImage::transitionComputeToFragmentRead(vk::CommandBuffer buffer)
{
    mComputeFrameReady = false;
    _transitionImageLayout(buffer, vk::ImageLayout::eGeneral, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void TextureImage::transitionRevertToCompute(vk::CommandBuffer buffer)
{
    mComputeFrameReady = true;
    _transitionImageLayout(buffer, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eGeneral);
}

TextureImageView::TextureImageView(const Device& device, const vk::Image image, vk::Format format)
{
    vk::ImageViewCreateInfo createInfo{};
    createInfo.setImage(image);
    createInfo.setViewType(vk::ImageViewType::e2D);
    createInfo.setFormat(format);

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
