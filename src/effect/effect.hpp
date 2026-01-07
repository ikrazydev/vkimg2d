#pragma once

#include <string>
#include <string_view>
#include <vector>

struct EffectParameter
{

};

class Effect
{
public:
    Effect(std::string_view id, std::string_view displayName, std::string_view shaderPath);

    [[nodiscard]] const std::string& getId() const noexcept;
    [[nodiscard]] const std::string& getDisplayName() const noexcept;
    [[nodiscard]] const std::string& getShaderPath() const noexcept;
private:
    std::string mId;
    std::string mDisplayName;
    std::string mShaderPath;

    std::vector<EffectParameter> mParameters;
};
