#pragma once

#include <string>
#include <string_view>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct ImageLoadResult
{
	int texWidth;
	int texHeight;
	int texChannels;

	stbi_uc* pixels;
};

class Image
{
public:
	Image(const std::string_view path);
	~Image();

	ImageLoadResult load();
private:
	static ImageLoadResult loadFromPath(const std::string_view path);

	std::string mPath;
	std::optional<ImageLoadResult> mLoadResult;
};
