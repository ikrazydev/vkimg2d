#include "compute_pipeline.hpp"

#include <vulkan/device.hpp>

ComputePipeline::ComputePipeline(const Device& device, const ComputePipelineConfig& config)
{
    vk::ComputePipelineCreateInfo pipelineInfo{};

    mPipeline = device.getVkHandle().createComputePipelineUnique(nullptr, pipelineInfo).value;
}
