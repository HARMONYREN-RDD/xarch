#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include <iostream>

std::vector<std::string> tokenize_line(std::string line) {
  std::vector<std::string> tokens;

  std::stringstream ss(line);
  std::string token;
  while (ss >> token) {
    tokens.push_back(token);
  }

  return tokens;
}

void compile_file(std::string file_path) {
  std::fstream file(file_path, std::ios::in);

  std::uint32_t x = 0;
  std::uint32_t y = 0;

  std::string line;
  while (std::getline(file, line)) {
    line.erase(line.begin(),
               std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                 return !std::isspace(ch);
               }));
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::vector<std::string> tokens = tokenize_line(line);
    for (int i = 0; i < tokens.size(); i++) {
      std::cout << tokens[i] << "\n";
    }
    std::cout << std::endl;

    y++;
  }
}

int main() {
  compile_file("test.xasm");

  return 0;
}