#include "registry.hpp"

EffectRegistry::EffectRegistry()
{
    mEffects.reserve(16U); // this is intentionally ugly and random

    Effect grayscale{ EffectIds::Grayscale, "Grayscale", "shaders/effects/grayscale.spv" };
    Effect invert{ EffectIds::Invert, "Invert", "shaders/effects/invert.spv" };
    Effect sepia{ EffectIds::Sepia, "Sepia", "shaders/effects/sepia.spv" };

    Effect posterize{ EffectIds::Posterize, "Posterize", "shaders/effects/posterize.spv" };
    posterize.addParam(FloatParam{
        .id = "level",
        .displayName = "Level",
        .defaultValue = 4.0f,
        .min = 0.0f, .max = 8.0f,
    });

    Effect solarize{ EffectIds::Solarize, "Solarize", "shaders/effects/solarize.spv" };
    solarize.addParam(FloatParam{
        .id = "threshold",
        .displayName = "Threshold",
        .defaultValue = 0.5f,
        .min = 0.0f, .max = 1.0f,
    });

    Effect threshold{ EffectIds::Threshold, "Threshold", "shaders/effects/threshold.spv" };
    threshold.addParam(FloatParam{
        .id = "threshold",
        .displayName = "Threshold",
        .defaultValue = 0.5f,
        .min = 0.0f, .max = 1.0f,
    });

    Effect briCon{ EffectIds::BriCon, "Brightness/Contrast", "shaders/effects/bricon.spv" };
    briCon.addParam(FloatParam{
        .id = "brightness",
        .displayName = "Brightness",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });
    briCon.addParam(FloatParam{
        .id = "contrast",
        .displayName = "Contrast",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });

    Effect hueSat{ EffectIds::HueSat, "Hue/Saturation", "shaders/effects/huesat.spv" };
    hueSat.addParam(FloatParam{
        .id = "hue",
        .displayName = "Hue",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });
    hueSat.addParam(FloatParam{
        .id = "saturation",
        .displayName = "Saturation",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });
    hueSat.addParam(FloatParam{
        .id = "brightness",
        .displayName = "Brightness",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });

    Effect colOffset{ EffectIds::ColOffset, "Color Offset", "shaders/effects/coloffset.spv" };
    colOffset.addParam(FloatParam{
        .id = "red_offset",
        .displayName = "Red Offset",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });
    colOffset.addParam(FloatParam{
        .id = "green_offset",
        .displayName = "Green Offset",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });
    colOffset.addParam(FloatParam{
        .id = "blue_offset",
        .displayName = "Blue Offset",
        .defaultValue = 0.0f,
        .min = -1.0f, .max = 1.0f,
    });

    mEffects.push_back(grayscale);
    mEffects.push_back(invert);
    mEffects.push_back(sepia);

    mEffects.push_back(posterize);
    mEffects.push_back(solarize);
    mEffects.push_back(threshold);

    mEffects.push_back(briCon);
    mEffects.push_back(hueSat);
    mEffects.push_back(colOffset);
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
