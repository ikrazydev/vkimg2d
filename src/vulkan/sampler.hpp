#pragma once

#include <vulkan/include.hpp>

class Device;

struct SamplerConfig
{
};

class Sampler
{
public:
    Sampler(const Device& device, const SamplerConfig& config);

    const vk::Sampler getVkHandle() const;
private:
    vk::UniqueSampler mSampler;
};
