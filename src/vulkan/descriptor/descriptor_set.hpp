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

    const TextureImage& texture;
    const Sampler& sampler;

    size_t count;
};

class DescriptorSet
{
public:
    DescriptorSet(const Device& device, const DescriptorSetConfig& config);

    const vk::DescriptorSet getVkHandle(size_t index) const;
private:
    std::vector<vk::UniqueDescriptorSet> mSets;
};
