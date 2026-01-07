#pragma once

#include <vector>

#include <effect/effect.hpp>

struct RegisteredEffect
{

};

class EffectRegistry
{
public:
    EffectRegistry();

    [[nodiscard]] const std::vector<Effect>& getEffects() const noexcept;
private:
    std::vector<Effect> mEffects;
};
