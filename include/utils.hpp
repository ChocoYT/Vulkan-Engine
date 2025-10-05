#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef UTILS_HPP
#define UTILS_HPP

std::string       readFile(const std::string &filePath);
std::vector<char> readFileBinary(const std::string &filePath);

#endif