#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <effect/param.hpp>

class Effect
{
public:
    Effect(std::string_view id, std::string_view displayName, const std::filesystem::path& shaderPath);

    [[nodiscard]] const std::string& getId() const noexcept;
    [[nodiscard]] const std::string& getDisplayName() const noexcept;
    [[nodiscard]] const std::filesystem::path& getShaderPath() const noexcept;
    [[nodiscard]] const std::vector<FloatParam>& getParams() const noexcept;

    const FloatParam* getParamById(std::string_view id) const;

    void addParam(FloatParam param);
private:
    std::string mId;
    std::string mDisplayName;
    std::filesystem::path mShaderPath;

    std::vector<FloatParam> mParams;
};
