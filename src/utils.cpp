#include "Utils.hpp"

std::string readFileString(const std::string &filePath)
{
    std::ifstream file(filePath.c_str());
    
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + filePath);
    
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();

    return source;
}

std::vector<char> readFileBinary(const std::string &filePath)
{
    std::ifstream file(filePath.c_str(), std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + filePath);

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}