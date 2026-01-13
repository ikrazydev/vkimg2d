#pragma once

#include <filesystem>
#include <vector>

class BinaryReader
{
public:
    static std::vector<char> readFromPath(const std::filesystem::path& filepath);
    static std::filesystem::path toShaderBinPath(const std::filesystem::path& filepath);
};
