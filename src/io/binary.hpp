#pragma once

#include <fstream>
#include <vector>
#include <string>

class BinaryReader
{
public:
    static std::vector<char> readFromPath(const std::string& filepath);
};
