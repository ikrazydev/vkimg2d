#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <string_view>

class BinaryReader
{
public:
    static std::vector<char> readFromPath(std::string_view filepath);
    static std::string toShaderBinPath(std::string_view filepath);
};
