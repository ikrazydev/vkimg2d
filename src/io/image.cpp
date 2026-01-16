#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image(const std::filesystem::path& path)
    : mPath{ path }
{
}

Image::~Image()
{
    if (mLoadResult.has_value()) {
        stbi_image_free(mLoadResult.value().pixels);
    }
}

ImageLoadResult Image::load()
{
    auto result = _loadFromPath(mPath);
    mLoadResult = result;

    return result;
}

ImageLoadResult Image::_loadFromPath(const std::filesystem::path& path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    return ImageLoadResult{
        .texWidth = texWidth,
        .texHeight = texHeight,
        .texChannels = texChannels,

        .pixels = pixels,
    };
}
