#pragma once

#include <effect/effect.hpp>

struct EffectInstance
{
    const Effect* effect;
    bool enabled = true;
};
