#pragma once

#include <unordered_map>

#include <effect/effect.hpp>

struct EffectInstance
{
    const Effect* effect;
    bool enabled = true;

    std::unordered_map<std::string, float> params{};

    EffectInstance(const Effect* effect);
private:
    void _loadDefaultParams();
};
