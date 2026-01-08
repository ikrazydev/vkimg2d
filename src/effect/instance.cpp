#include "instance.hpp"

EffectInstance::EffectInstance(const Effect* effect)
: effect{ effect }
{
    _loadDefaultParams();
}

std::vector<float> EffectInstance::getParamValues() const
{
    std::vector<float> result{};
    result.reserve(params.size());

    for (const auto& pair : params) {
        result.push_back(pair.second);
    }

    return result;
}

void EffectInstance::_loadDefaultParams()
{
    params.clear();

    for (const auto& param : effect->getParams()) {
        params[param.id] = param.defaultValue;
    }
}
