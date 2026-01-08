#pragma once

#include <string>

struct FloatParam
{
    std::string id;
    std::string displayName;

    float defaultValue;
    float min, max;
};
