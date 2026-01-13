#pragma once

#include <filesystem>

namespace Paths
{
    inline const std::filesystem::path Samples{ "samples" };
    inline const std::filesystem::path Shaders{ "shaders" };
    inline const std::filesystem::path ShadersBin{ Shaders / "bin" };
    inline const std::filesystem::path Presets{ "presets" };
}
