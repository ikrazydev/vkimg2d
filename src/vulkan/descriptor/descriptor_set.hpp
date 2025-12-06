#pragma once

#include <vector>

#include <vulkan/include.hpp>
#include <vulkan/sampler.hpp>
#include <vulkan/buffer/texture.hpp>
#include <vulkan/descriptor/descriptor_layout.hpp>
#include <vulkan/descriptor/descriptor_pool.hpp>

struct DescriptorSetConfig
{
    const DescriptorLayout& descriptorLayout;
    const DescriptorPool& descriptorPool;

    const TextureImage& texture;
    const Sampler& sampler;

    size_t count;
};

class Device;

class DescriptorSet
{
public:
    DescriptorSet(const Device& device, const DescriptorSetConfig& config);
private:
    std::vector<vk::UniqueDescriptorSet> mSets;
};
