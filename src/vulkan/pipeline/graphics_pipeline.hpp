#pragma once

#include <string_view>

#include <vulkan/include.hpp>

class Device;
class Renderpass;
class DescriptorLayout;

struct GraphicsPipelineConfig
{
    std::string_view vertexShaderPath;
    std::string_view fragmentShaderPath;

    vk::Extent2D swapchainExtent;

    const Renderpass& renderpass;
    uint32_t subpass;

    const DescriptorLayout& descriptorLayout;
};

class GraphicsPipeline
{
public:
    GraphicsPipeline(const Device& device, const GraphicsPipelineConfig& config);

    const vk::Pipeline getVkHandle() const;
    const vk::PipelineLayout getLayout() const;
private:
    const Device& mDevice;
    const GraphicsPipelineConfig& mConfig;

    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;
};
