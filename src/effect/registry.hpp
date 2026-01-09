#pragma once

#include <string_view>
#include <vector>

#include <effect/effect.hpp>

namespace EffectIds
{
    inline constexpr std::string_view Grayscale = "grayscale";
    inline constexpr std::string_view Invert = "invert";
    inline constexpr std::string_view Sepia = "sepia";

    inline constexpr std::string_view Posterize = "posterize";
    inline constexpr std::string_view Solarize = "solarize";
    inline constexpr std::string_view Threshold = "threshold";

    inline constexpr std::string_view BriCon = "bri_con";
    inline constexpr std::string_view HueSat = "hue_sat";
    inline constexpr std::string_view ColOffset = "color_offset";
}

class EffectRegistry
{
public:
    EffectRegistry();

    [[nodiscard]] const std::vector<Effect>& getEffects() const noexcept;
    [[nodiscard]] const Effect* getById(std::string_view id) const noexcept;
private:
    std::vector<Effect> mEffects;
};
