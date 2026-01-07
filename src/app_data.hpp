#pragma once

#include <vector>

#include <effect/registry.hpp>
#include <effect/instance.hpp>

struct AppData
{
    EffectRegistry registry;
    std::vector<EffectInstance> effects;

    float mix = 1.0f;

    void addEffect(const Effect* effect);
};
