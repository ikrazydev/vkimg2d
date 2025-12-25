#pragma once

#include <vulkan/include.hpp>

#include <string_view>
#include <vector>

class Device;

enum class ShaderType
{
    Vertex,
    Fragment,
    Compute,
};

struct ShaderConfig
{
    ShaderType type;
};

class Shader
{
public:
    Shader(const Device& device, std::string_view filepath, const ShaderConfig& config);

    const vk::ShaderModule& getModule() const;
    const vk::PipelineShaderStageCreateInfo& getStageInfo() const;
private:
    void _createShaderModule(const std::vector<char>& bytecode);

    static vk::ShaderStageFlagBits _convertShaderType(const ShaderType& type);
    void _createShaderStageInfo(const ShaderConfig& config);

    const Device& mDevice;
    vk::UniqueShaderModule mModule;
    vk::PipelineShaderStageCreateInfo mStageInfo;
};
