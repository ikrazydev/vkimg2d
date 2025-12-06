#include "sampler.hpp"

#include <vulkan/device.hpp>

Sampler::Sampler(const Device& device, const SamplerConfig& config)
{
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;

    auto deviceProps = device.getPhysicalDevice().getProperties();

    samplerInfo.anisotropyEnable = vk::True;
    samplerInfo.maxAnisotropy = deviceProps.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = vk::False;
    samplerInfo.compareEnable = vk::False;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    mSampler = device.getVkHandle().createSamplerUnique(samplerInfo);
}

const vk::Sampler Sampler::getVkHandle() const
{
    return mSampler.get();
}
