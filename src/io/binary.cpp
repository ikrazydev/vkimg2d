#include "binary.hpp"

#include <fstream>

#include <io/path.hpp>

std::vector<char> BinaryReader::readFromPath(const std::filesystem::path& filepath)
{
    std::ifstream file(filepath.string(), std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file.");
    }

    auto fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

std::filesystem::path BinaryReader::toShaderBinPath(const std::filesystem::path& filepath)
{
    return Paths::ShadersBin / filepath;
}
