#include "binary.hpp"

#include <format>

std::vector<char> BinaryReader::readFromPath(std::string_view filepath)
{
    std::ifstream file(filepath.data(), std::ios::ate | std::ios::binary);

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

std::string BinaryReader::toShaderBinPath(std::string_view filepath)
{
    return std::format("shaders/bin/{}", filepath);
}
