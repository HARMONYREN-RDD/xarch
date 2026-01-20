#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cstdint>

#include <xasm/error.hpp>
using namespace xasm;

// Convert decimal or 16_hex format to uint64_t
std::expected<std::uint64_t, FatalError> string_to_uint(const std::string& n_str) {
  std::uint64_t n = 0;

  if (n_str.size() >= 4 && n_str[0] == '1' && n_str[1] == '6' && n_str[2] == '_') {
    std::string hex_part = n_str.substr(3);
    if (hex_part.empty()) return std::unexpected{ "Empty hex value" };

    for (char ch : hex_part) {
      n *= 16;
      if (ch >= '0' && ch <= '9') n += ch - '0';
      else if (ch >= 'A' && ch <= 'F') n += 10 + (ch - 'A');
      else if (ch >= 'a' && ch <= 'f') n += 10 + (ch - 'a');
      else return std::unexpected{ "Invalid hex character" };
    }
  }
  else {
    for (char ch : n_str) {
      if (!isdigit(ch)) return std::unexpected("Characters are not admitted");
      n = n * 10 + (ch - '0');
    }
  }

  return n;
}

struct Parameter {
  enum class Type { REGISTER, VALUE } type;
  std::uint16_t byte_width; // bytes used for actual value
};

struct Instruction {
  std::uint16_t opcode;
  std::vector<Parameter> parameters;
};

using InstructionSet = std::unordered_map<std::string, Instruction>;

std::expected<uint32_t, FatalError> compile_file(std::ifstream& file, const InstructionSet& is) {
  std::vector<std::uint8_t> output;
  std::string line;
  std::uint64_t line_number = 1;

  while (std::getline(file, line)) {
    if (line.empty()) { line_number++; continue; }

    std::istringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (ss >> token) {
      if (token == "#") break; // comment
      tokens.push_back(token);
    }

    if (tokens.empty()) { line_number++; continue; }

    std::string mnemonic = tokens[0];
    auto it = is.find(mnemonic);
    if (it == is.end())
      Err("Mnemonic '{}' does not exist", mnemonic);

    auto& inst = it->second;

    // write opcode (2 bytes, little-endian)
    output.push_back(inst.opcode & 0xFF);
    output.push_back(inst.opcode >> 8);

    size_t operand_idx = 0;
    for (; operand_idx < inst.parameters.size(); ++operand_idx) {
      if (operand_idx + 1 >= tokens.size())
        Err("Missing operand for '{}'", mnemonic);

      token = tokens[operand_idx + 1];
      uint64_t value = 0;

      auto& param = inst.parameters[operand_idx];
      switch (param.type) {
      case Parameter::Type::REGISTER:
        if (token.size() < 2)
          Err("Register token too short '{}'", token);

        if (token[1] >= '0' && token[1] <= '9') value = token[1] - '0';
        else if (token[1] >= 'A' && token[1] <= 'F') value = 10 + (token[1] - 'A');
        else Err("Invalid register number '{}'", token);

        break;

      case Parameter::Type::VALUE:
        value = unwrap_or_fatal(string_to_uint(token));

        break;

      default:
        Err("Unexpected parameter type");
      }

      for (int i = 0; i < 8; ++i) {
        output.push_back(static_cast<uint8_t>(value & 0xFF));
        value >>= 8;
      }
    }


    for (size_t i = 0; i < 3 - inst.parameters.size(); i++)
      for (int j = 0; j < 8; j++)
        output.push_back(0x00);

    line_number++;
  }

  std::ofstream out("out.bin", std::ios::binary);
  out.write(reinterpret_cast<const char*>(output.data()), static_cast<std::streamsize>(output.size()));
  out.close();

  return 0;
}


int main() {
  std::ifstream file("test.xasm");
  if (!file.is_open()) { std::cerr << "Cannot open file\n"; return 1; }

  InstructionSet instruction_set;

  instruction_set["setq"] = { 0x0003, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::VALUE, 8}} };
  instruction_set["cpq"] = { 0x0007, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}} };

  instruction_set["loadq"] = { 0x0013, { {Parameter::Type::VALUE, 8}, {Parameter::Type::VALUE, 8}} };
  instruction_set["storeq"] = { 0x0017, { {Parameter::Type::VALUE, 8}, {Parameter::Type::VALUE, 8}} };

  instruction_set["addqrr"] = { 0x0030, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}} };
  instruction_set["addqrv"] = { 0x0031, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::VALUE, 8}} };
  instruction_set["subqrr"] = { 0x0032, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}} };
  instruction_set["subqrv"] = { 0x0033, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::VALUE, 8}} };
  instruction_set["mulqrr"] = { 0x0034, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}} };
  instruction_set["mulqrv"] = { 0x0035, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::VALUE, 8}} };
  instruction_set["divqrr"] = { 0x0036, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}} };
  instruction_set["divqrv"] = { 0x0037, { {Parameter::Type::REGISTER, 8}, {Parameter::Type::REGISTER, 8}, {Parameter::Type::VALUE, 8}} };


  instruction_set["print"] = { 0xFFFF, {} };

  unwrap_or_fatal(compile_file(file, instruction_set));

  return 0;
}
