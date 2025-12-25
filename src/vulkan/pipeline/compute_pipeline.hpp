#pragma once

#include <vulkan/include.hpp>

class Device;

struct ComputePipelineConfig
{

};

class ComputePipeline
{
public:
	ComputePipeline(const Device& device, const ComputePipelineConfig& config);
private:
	vk::UniquePipelineLayout mPipelineLayout;
	vk::UniquePipeline mPipeline;
};
