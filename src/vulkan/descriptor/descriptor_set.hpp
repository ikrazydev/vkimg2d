#pragma once

#include <vector>

#include <vulkan/include.hpp>
#include <vulkan/sampler.hpp>
#include <vulkan/buffer/texture.hpp>
#include <vulkan/descriptor/descriptor_layout.hpp>
#include <vulkan/descriptor/descriptor_pool.hpp>

class Device;

struct DescriptorSetConfig
{
    const DescriptorLayout& descriptorLayout;
    const DescriptorPool& descriptorPool;
};

struct DescriptorSetImage
{
    uint32_t binding;
    const TextureImage& texture;
    const Sampler* sampler;
    vk::ImageLayout layout;
    vk::DescriptorType descriptorType;
};

struct DescriptorUpdateConfig
{
    const std::vector<DescriptorSetImage>& images;
};

class DescriptorSet
{
public:
    DescriptorSet(const Device& device, const DescriptorSetConfig& config);

    DescriptorSet(DescriptorSet&&) = default;
    DescriptorSet& operator=(DescriptorSet&&) = default;

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;

    void update(const DescriptorUpdateConfig& config) const;

    [[nodiscard]] vk::DescriptorSet getVkHandle() const noexcept;
private:
    const Device& mDevice;

    vk::UniqueDescriptorSet mSet;
};
