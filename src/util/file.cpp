#include "util/file.hpp"

// Includes

#include <fstream>
#include <iostream>

// File functions

void save_to_file(const std::string& file, const std::vector<float>& values) {
   std::ofstream f {file};
   for (auto& v : values) {
      f << std::to_string(v) << '\n';
   }
}

std::vector<float> read_from_file(const std::string& file, const std::vector<float>& defaults) {
   std::ifstream f {file};
   std::vector<float> values;
   std::string line;

   while (std::getline(f, line)) {
      try {
         values.push_back(std::stof(line));
      } catch (...) {
         values.push_back(0);
      }
   }

   for (int i = values.size(); i < defaults.size(); ++i) {
      std::cout << i << values.size() << defaults.size() << '\n';
      values.push_back(defaults[i]);
   }
   return values;
}
