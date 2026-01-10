#include "app_data.hpp"

void AppData::addEffect(const Effect* effect)
{
    EffectInstance inst{ effect };

    this->effects.push_back(inst);
}

void AppData::deleteEffect(const size_t index)
{
    effects.erase(std::next(effects.begin(), index));
}

void AppData::moveUpEffect(const size_t index)
{
    if (index == 0) return;

    std::rotate(effects.rend() - index - 1, effects.rend() - index, effects.rend() - index + 1);
}

void AppData::moveDownEffect(const size_t index)
{
    if (index >= effects.size() - 1) return;

    std::rotate(effects.begin() + index, effects.begin() + index + 1, effects.begin() + index + 2);
}
