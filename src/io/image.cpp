#include "image.hpp"

Image::Image(const std::string_view path)
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
    auto result = loadFromPath(mPath);
    mLoadResult = result;

    return result;
}

ImageLoadResult Image::loadFromPath(const std::string_view path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    return ImageLoadResult{
        .texWidth = texWidth,
        .texHeight = texHeight,
        .texChannels = texChannels,

        .pixels = pixels,
    };
}
