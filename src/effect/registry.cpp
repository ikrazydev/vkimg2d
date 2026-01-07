#include "registry.hpp"

EffectRegistry::EffectRegistry()
{
    mEffects.reserve(16U); // this is intentionally ugly and random

    Effect grayscale{ "grayscale", "Grayscale", "shaders/effects/grayscale.glsl" };
    Effect invert{ "invert", "Invert", "shaders/effects/invert.glsl" };
    Effect sepia{ "sepia", "Sepia", "shaders/effects/sepia.glsl" };

    mEffects.push_back(grayscale);
    mEffects.push_back(invert);
    mEffects.push_back(sepia);
}

const std::vector<Effect>& EffectRegistry::getEffects() const noexcept
{
    return mEffects;
}
