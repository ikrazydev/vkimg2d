#include "registry.hpp"

EffectRegistry::EffectRegistry()
{
    mEffects.reserve(16U); // this is intentionally ugly and random

    Effect grayscale{ EffectIds::Grayscale, "Grayscale", "shaders/effects/grayscale.spv" };
    Effect invert{ EffectIds::Invert, "Invert", "shaders/effects/invert.spv" };
    Effect sepia{ EffectIds::Sepia, "Sepia", "shaders/effects/sepia.spv" };

    mEffects.push_back(grayscale);
    mEffects.push_back(invert);
    mEffects.push_back(sepia);
}

const std::vector<Effect>& EffectRegistry::getEffects() const noexcept
{
    return mEffects;
}

const Effect* EffectRegistry::getById(std::string_view id) const noexcept
{
    auto it = std::ranges::find_if(mEffects, [id](const Effect& e) {
        return e.getId() == id;
    });

    return it != mEffects.end() ? std::to_address(it) : nullptr;
}
