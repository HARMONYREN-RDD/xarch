#include <iostream>

#include <filesystem>
#include <fstream>
#include <vector>
#include <ranges>
#include <unordered_map>


namespace fs = std::filesystem;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u64 = std::uint64_t;

using String = std::string;

using Path = fs::path;

template <typename T> using Vec = std::vector<T>;
template <typename T1, typename T2> using Map = std::unordered_map<T1, T2>;


enum class ParameterType : u8 {
  REGISTER, VALUE
};

struct Parameter {
  ParameterType type;
  u8 byte_size;
  std::string label;
};

struct Instruction {
  u16 opcode;
  Vec<Parameter> parameters;
};

using InstructionSet = Map<String, Instruction>;

void compile_xasm_file(const Path& input_file_path, Vec<u8>& output, const InstructionSet& instruction_set);
void dump_to_file(const Path& output_file_path, const Vec<u8>& output);


int main() {
  Path file_path("./test.xasm");
  InstructionSet is;

  // 第0章：記憶口作戦
  is["seto"] = { 0x0003, {{ParameterType::REGISTER, 8}, {ParameterType::VALUE, 8} } };
  is["cpo"] = { 0x0007, {{ParameterType::REGISTER, 8}, {ParameterType::REGISTER, 8} } };

  // 第１章：主記憶作戦
  is["storeo"] = { 0x0013, {{ParameterType::VALUE, 64}, {ParameterType::VALUE, 64}} };
  is["loado"] = { 0x0017, {{ParameterType::REGISTER, 64}, {ParameterType::VALUE, 64}} };

  // 第２章：算術
  is["addorr"] = { 0x0020, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}} };
  is["addorv"] = { 0x0021, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::VALUE, 64}} };
  is["suborr"] = { 0x0022, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}} };
  is["suborv"] = { 0x0023, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::VALUE, 64}} };
  is["mulorr"] = { 0x0024, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}} };
  is["mulorv"] = { 0x0025, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::VALUE, 64}} };
  is["divorr"] = { 0x0026, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}} };
  is["divorv"] = { 0x0027, {{ParameterType::REGISTER, 64}, {ParameterType::REGISTER, 64}, {ParameterType::VALUE, 64}} };


  // 第特殊章：特殊作戦
  is["print"] = { 0xFFFF, {} };

  Vec<u8> output;
  compile_xasm_file(file_path, output, is);
  dump_to_file("out.xb", output);

  return 0;
}

void compile_tokens(const Vec<String>& tokens, const InstructionSet& instruction_set, Vec<u8>& output) {
  auto it = instruction_set.find(tokens[0]);
  if (it == instruction_set.end()) {
    std::cout << "Mnemonic " << tokens[0] << " not found" << std::endl;
    std::exit(1);
  }

  auto& instruction = it->second;
  auto& opcode = instruction.opcode;
  output.push_back(opcode & 0xFF);
  output.push_back(opcode >> 8);


  if (tokens.size() - 1 < instruction.parameters.size()) {
    std::cout << "Too few params" << std::endl;
    std::exit(1);
  }

  if (tokens.size() - 1 > instruction.parameters.size()) {
    std::cout << "Too many params" << std::endl;
    std::exit(1);
  }

  u8 parameter_index = 1;

  for (auto& parameter : instruction.parameters) {
    auto operand = tokens[parameter_index];

    if (parameter.type == ParameterType::VALUE) {
      if (parameter.byte_size == 8) {
        u64 out_u64 = 0;
        if (operand.size() > 3 && (operand[0] == '1' && operand[1] == '6' && operand[2] == '_')) {
          operand = operand.substr(3);
          out_u64 = std::stoull(operand, nullptr, 16);
        }
        else {
          out_u64 = std::stoull(operand, nullptr, 10);
        }

        for (size_t i = 0; i < 8; ++i) {
          output.push_back(static_cast<uint8_t>(out_u64 >> (i * 8)));
        }

      }
    }

    parameter_index++;
  }

}

void compile_xasm_file(const Path& input_file_path, Vec<u8>& output, const InstructionSet& instruction_set) {
  std::ifstream file(input_file_path);
  String line = "";
  std::uint64_t line_number = 0;

  while (std::getline(file, line)) {
    line_number++;

    std::istringstream iss(line);
    Vec<String> tokens;
    String token = "";

    while (iss >> token) {
      if (token[0] == '#')
        break;
      tokens.push_back(token);
    }

    compile_tokens(tokens, instruction_set, output);
  }
}

void dump_to_file(const Path& output_file_path, const Vec<u8>& output) {
  std::ofstream out_file(output_file_path, std::ios::binary);
  if (!out_file)
    throw std::runtime_error("Failed to open file: " + output_file_path.string());
  out_file.write(reinterpret_cast<const char*>(output.data()), output.size());
}
