#include "compute_pipeline.hpp"

#include <vulkan/device.hpp>
#include <vulkan/descriptor/descriptor_layout.hpp>
#include <vulkan/shader.hpp>

ComputePipeline::ComputePipeline(const Device& device, const ComputePipelineConfig& config)
    : mDevice{ device }
{
    ShaderConfig shaderConfig{ .type = ShaderType::Compute };
    Shader shader{ mDevice, config.shaderPath, shaderConfig };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    const auto descriptorLayout = config.descriptorLayout.getVkHandle();
    pipelineLayoutInfo.setSetLayouts(descriptorLayout);

    if (config.usePushConstants) {
        vk::PushConstantRange pushConstantRange{};
        pushConstantRange.setOffset(0U);
        pushConstantRange.setSize(config.pushConstantSize);
        pushConstantRange.setStageFlags(vk::ShaderStageFlagBits::eCompute);

        pipelineLayoutInfo.setPushConstantRanges(pushConstantRange);
    }
    else {
        pipelineLayoutInfo.setPushConstantRanges(nullptr);
    }

    mPipelineLayout = mDevice.getVkHandle().createPipelineLayoutUnique(pipelineLayoutInfo);

    vk::ComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStage(shader.getStageInfo());
    pipelineInfo.setLayout(mPipelineLayout.get());

    mPipeline = device.getVkHandle().createComputePipelineUnique(nullptr, pipelineInfo).value;
}

const vk::Pipeline ComputePipeline::getVkHandle() const
{
    return mPipeline.get();
}

const vk::PipelineLayout ComputePipeline::getLayout() const
{
    return mPipelineLayout.get();
}
