#include "effect.hpp"

#include <ranges>

Effect::Effect(std::string_view id, std::string_view displayName, const std::filesystem::path& shaderPath)
    : mId{ id }
    , mDisplayName{ displayName }
    , mShaderPath{ shaderPath }
{
}

const std::string& Effect::getId() const noexcept
{
    return mId;
}

const std::string& Effect::getDisplayName() const noexcept
{
    return mDisplayName;
}

const std::filesystem::path& Effect::getShaderPath() const noexcept
{
    return mShaderPath;
}

const std::vector<FloatParam>& Effect::getParams() const noexcept
{
    return mParams;
}

const FloatParam* Effect::getParamById(std::string_view id) const
{
    auto it = std::ranges::find_if(mParams, [id](const FloatParam& e) {
        return e.id == id;
    });

    return it != mParams.end() ? std::to_address(it) : nullptr;
}

void Effect::addParam(FloatParam param)
{
    mParams.push_back(param);
}
