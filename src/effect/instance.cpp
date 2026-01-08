#include "instance.hpp"

EffectInstance::EffectInstance(const Effect* effect)
: effect{ effect }
{
    _loadDefaultParams();
}

void EffectInstance::_loadDefaultParams()
{
    params.clear();

    for (const auto& param : effect->getParams()) {
        params[param.id] = param.defaultValue;
    }
}
