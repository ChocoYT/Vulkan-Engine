#include "utils.hpp"

std::string readFile(const std::string filePath)
{
    std::ifstream file(filePath.c_str());
    
    if (!file.is_open())
        std::runtime_error("Could not open file: " + filePath);
    
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    return source;
}
