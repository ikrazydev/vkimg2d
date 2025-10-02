#pragma once

#include <fstream>
#include <vector>
#include <string_view>

class BinaryReader
{
public:
    static std::vector<char> readFromPath(const std::string_view filepath);
};
