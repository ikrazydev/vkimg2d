#include "graphics_pipeline.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/shader.hpp>
#include <vulkan/vertex.hpp>
#include <vulkan/descriptor/descriptor_layout.hpp>

GraphicsPipeline::GraphicsPipeline(const Device& device, const GraphicsPipelineConfig& config)
    : mDevice{ device }
    , mConfig{ config }
{
    ShaderConfig vertexShaderConfig{ .type = ShaderType::Vertex };
    ShaderConfig fragmentShaderConfig{ .type = ShaderType::Fragment };

    Shader vertexShader{ mDevice, mConfig.vertexShaderPath, vertexShaderConfig };
    Shader fragmentShader{ mDevice, mConfig.fragmentShaderPath, fragmentShaderConfig };
    std::array shaderStages{ vertexShader.getStageInfo(), fragmentShader.getStageInfo() };

    std::array dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.setDynamicStates(dynamicStates);

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth((float)mConfig.swapchainExtent.width);
    viewport.setHeight((float)mConfig.swapchainExtent.height);
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    vk::Rect2D scissor{};
    scissor.setOffset(vk::Offset2D{ 0U, 0U });
    scissor.setExtent(mConfig.swapchainExtent);

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewports(viewport);
    viewportState.setScissors(scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setDepthClampEnable(vk::False);
    rasterizer.setRasterizerDiscardEnable(vk::False);
    rasterizer.setPolygonMode(vk::PolygonMode::eFill);
    rasterizer.setLineWidth(1.0f);
    rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
    rasterizer.setFrontFace(vk::FrontFace::eClockwise);

    rasterizer.setDepthBiasEnable(vk::False);
    rasterizer.setDepthBiasConstantFactor(vk::False);
    rasterizer.setDepthBiasSlopeFactor(0.0f);
    rasterizer.setDepthBiasClamp(vk::False);

    vk::PipelineMultisampleStateCreateInfo multisample{};
    multisample.setSampleShadingEnable(vk::False);
    multisample.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisample.setMinSampleShading(1.0f);
    multisample.setPSampleMask(nullptr);
    multisample.setAlphaToCoverageEnable(vk::False);
    multisample.setAlphaToOneEnable(vk::False);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.setBlendEnable(vk::False);
    colorBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eOne);
    colorBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eZero);
    colorBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
    colorBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    colorBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    colorBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable(vk::False);
    colorBlending.setLogicOp(vk::LogicOp::eCopy);
    colorBlending.setAttachments(colorBlendAttachment);
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    const auto descriptorLayout = config.descriptorLayout.getVkHandle();
    pipelineLayoutInfo.setSetLayouts(descriptorLayout);
    pipelineLayoutInfo.setPushConstantRanges(nullptr);

    mPipelineLayout = mDevice.getVkHandle().createPipelineLayoutUnique(pipelineLayoutInfo);

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptions(bindingDescription);
    vertexInputInfo.setVertexAttributeDescriptions(attributeDescriptions);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssembly.setPrimitiveRestartEnable(vk::False);

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStages(shaderStages);

    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisample);
    pipelineInfo.setPDepthStencilState(nullptr);
    pipelineInfo.setPColorBlendState(&colorBlending);
    pipelineInfo.setPDynamicState(&dynamicState);

    pipelineInfo.setLayout(mPipelineLayout.get());
    pipelineInfo.setRenderPass(config.renderpass.getVkHandle());
    pipelineInfo.setSubpass(config.subpass);

    pipelineInfo.setBasePipelineHandle(nullptr);
    pipelineInfo.setBasePipelineIndex(-1);

    mPipeline = mDevice.getVkHandle().createGraphicsPipelineUnique(nullptr, pipelineInfo).value;
}

const vk::Pipeline GraphicsPipeline::getVkHandle() const
{
    return mPipeline.get();
}

const vk::PipelineLayout GraphicsPipeline::getLayout() const
{
    return mPipelineLayout.get();
}
