#ifndef UTIL_FILE_HPP
#define UTIL_FILE_HPP

// Includes

#include <string>
#include <vector>

// File functions

void save_to_file(const std::string& file, const std::vector<float>& values);
std::vector<float> read_from_file(const std::string& file, const std::vector<float>& defaults);

#endif
