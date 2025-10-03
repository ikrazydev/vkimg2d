#pragma once

#include <vulkan/include.hpp>

#include <array>
#include <string_view>
#include <optional>
#include <vector>

class Device;
class Renderpass;

struct GraphicsPipelineConfig
{
    std::string_view vertexShaderPath;
    std::string_view fragmentShaderPath;

    vk::Extent2D swapchainExtent;

    const Renderpass& renderpass;
    uint32_t subpass;
};

class GraphicsPipeline
{
public:
    GraphicsPipeline(const Device& device, const GraphicsPipelineConfig& config);

    const vk::Pipeline& getVkHandle() const;
private:
    const Device& mDevice;
    const GraphicsPipelineConfig& mConfig;

    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;
};
