#include "graphics_pipeline.hpp"

#include <vulkan/device.hpp>
#include <vulkan/renderpass.hpp>
#include <vulkan/shader.hpp>

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
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)mConfig.swapchainExtent.width;
    viewport.height = (float)mConfig.swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vk::Rect2D scissor{};
    scissor.offset = vk::Offset2D{ 0U, 0U };
    scissor.extent = mConfig.swapchainExtent;

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1U;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1U;
    viewportState.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = vk::False;
    rasterizer.rasterizerDiscardEnable = vk::False;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;

    rasterizer.depthBiasEnable = vk::False;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = vk::False;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = vk::False;
    multisampling.alphaToOneEnable = vk::False;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = vk::False;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = vk::False;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    mPipelineLayout = mDevice.getVkHandle().createPipelineLayoutUnique(pipelineLayoutInfo);

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexBindingDescriptionCount = 1U;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = false;

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStages(shaderStages);

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = mPipelineLayout.get();
    pipelineInfo.renderPass = config.renderpass.getVkHandle();
    pipelineInfo.subpass = config.subpass;

    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex = -1;

    mPipeline = mDevice.getVkHandle().createGraphicsPipelineUnique(VK_NULL_HANDLE, pipelineInfo).value;
}

const vk::Pipeline& GraphicsPipeline::getVkHandle() const
{
    return mPipeline.get();
}
