#include "shader.hpp"

#include <io/binary.hpp>
#include <vulkan/device.hpp>

Shader::Shader(const Device& device, std::string_view filepath, const ShaderConfig& config)
    : mDevice{device}
    , mStageInfo{}
{
    auto shaderCode = BinaryReader::readFromPath(filepath);

    _createShaderModule(shaderCode);
    _createShaderStageInfo(config);
}

const vk::ShaderModule& Shader::getModule() const
{
    return mModule.get();
}

const vk::PipelineShaderStageCreateInfo& Shader::getStageInfo() const
{
    return mStageInfo;
}

void Shader::_createShaderModule(const std::vector<char>& bytecode)
{
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(bytecode.size());
    createInfo.setPCode(reinterpret_cast<const uint32_t*>(bytecode.data()));

    mModule = mDevice.getVkHandle().createShaderModuleUnique(createInfo);
}

vk::ShaderStageFlagBits Shader::_convertShaderType(const ShaderType& type)
{
    switch (type)
    {
    case ShaderType::Vertex:
        return vk::ShaderStageFlagBits::eVertex;
    case ShaderType::Fragment:
        return vk::ShaderStageFlagBits::eFragment;
    }
}

void Shader::_createShaderStageInfo(const ShaderConfig& config)
{
    mStageInfo.setStage(_convertShaderType(config.type));
    mStageInfo.setModule(mModule.get());
    mStageInfo.setPName("main");
}
