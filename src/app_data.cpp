#include "app_data.hpp"

void AppData::addEffect(const Effect* effect)
{
    EffectInstance inst{
        .effect = effect,
    };

    this->effects.push_back(inst);
}
