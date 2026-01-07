#include "effect.hpp"

Effect::Effect(std::string_view id, std::string_view displayName, std::string_view shaderPath)
    : mId{ id }
    , mDisplayName{ displayName }
    , mShaderPath{ shaderPath }
{
}

const std::string &Effect::getId() const noexcept
{
    return mId;
}

const std::string &Effect::getDisplayName() const noexcept
{
    return mDisplayName;
}

const std::string &Effect::getShaderPath() const noexcept
{
    return mShaderPath;
}
