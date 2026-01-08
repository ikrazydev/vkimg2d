#pragma once

#include <unordered_map>

#include <effect/effect.hpp>

struct EffectInstance
{
public:
    EffectInstance(const Effect* effect);

    const Effect* effect;
    bool enabled = true;

    std::unordered_map<std::string, float> params{};

    std::vector<float> getParamValues() const;
private:
    void _loadDefaultParams();
};
