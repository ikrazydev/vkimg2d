#pragma once

#include <string_view>

#include <vulkan/include.hpp>

class Device;
class DescriptorLayout;

struct ComputePipelineConfig
{
    std::string_view shaderPath;

    const DescriptorLayout& descriptorLayout;

    bool usePushConstants;
    uint32_t pushConstantSize;
};

class ComputePipeline
{
public:
    ComputePipeline(const Device& device, const ComputePipelineConfig& config);

    const vk::Pipeline getVkHandle() const;
    const vk::PipelineLayout getLayout() const;
private:
    const Device& mDevice;

    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;
};
