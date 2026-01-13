#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <optional>

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
	explicit Image(const std::filesystem::path& path);
	~Image();

	ImageLoadResult load();
private:
	static ImageLoadResult _loadFromPath(const std::filesystem::path& path);

	std::filesystem::path mPath;
	std::optional<ImageLoadResult> mLoadResult;
};
