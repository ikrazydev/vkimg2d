#pragma once

#include <unordered_map>

#include <vulkan/pipeline/compute_pipeline.hpp>

struct PipelineSet
{
    std::unordered_map<std::string, ComputePipeline> effectPipelines;
};
