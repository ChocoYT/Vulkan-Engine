#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

std::string       readFile(const std::string &filePath);
std::vector<char> readFileBinary(const std::string &filePath);
