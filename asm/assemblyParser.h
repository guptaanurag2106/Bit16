#pragma once

#include <getopt.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

enum InstructionType {
  NoParams = 0,
  Register_only = 1,
  Immediate_only = 2,
  Register_Immediate_only = 3,
  ALL_1 = 4,
  ALL_SW = 5
};

struct InstructionParams {
  uint8_t reg;
  uint8_t imm8;
  bool choice;
  InstructionParams() {}
  InstructionParams(uint8_t r, uint8_t i, bool c)
      : reg(r), imm8(i), choice(c) {}
};

// cout format for InstrucionParam
std::ostream& operator<<(std::ostream& os, const InstructionParams& param) {
  os << param.choice ? param.imm8 : param.reg;
  return os;
}

struct Instruction {
  std::string mnemonic;
  uint8_t opcode;
  InstructionType type;
  InstructionParams param1;
  InstructionParams param2;
  Instruction(){};
  Instruction(std::string m, uint8_t o, InstructionType t)
      : mnemonic(m), opcode(o), type(t) {}

  Instruction(std::string m, uint8_t o, InstructionType t, InstructionParams p1,
              InstructionParams p2)
      : mnemonic(m), opcode(o), type(t), param1(p1), param2(p2) {}
};

// cout format for Instrucion
std::ostream& operator<<(std::ostream& os, const Instruction& ins) {
  os << ins.mnemonic << " " << ins.param1 << ", " << ins.param2 << " "
     << ins.type;
  return os;
}

const std::map<std::string, InstructionType> instruction_set = {
    {"NOP", NoParams},
    {"HALT", NoParams},
    {"MW", ALL_1},
    {"MWL", Immediate_only},
    {"MWH", Immediate_only},
    {"LW", ALL_1},
    {"SW", ALL_SW},
    {"ADD", ALL_1},
    {"SUB", ALL_1},
    {"AND", ALL_1},
    {"OR", ALL_1},
    {"NOT", ALL_1},
    {"JMPZ", Register_Immediate_only},
    {"JMPN", Register_Immediate_only},
    {"PUSH", Register_Immediate_only},
    {"POP", Register_only},
};
const std::map<std::string, uint8_t> opcode_set = {
    {"NOP", 0x0},  {"HALT", 0x1}, {"MW", 0x2},   {"MWL", 0x3},
    {"MWH", 0x4},  {"LW", 0x5},   {"SW", 0x6},   {"ADD", 0x7},
    {"SUB", 0x8},  {"AND", 0x9},  {"OR", 0xa},   {"NOT", 0xb},
    {"JMPZ", 0xc}, {"JMPN", 0xd}, {"PUSH", 0xe}, {"POP", 0xf},
};

const std::set<std::string> register_set = {"A",  "B",  "C", "SR",
                                            "HL", "SP", "PC"};

class AssemblyParser {
 private:
  std::ifstream file;
  std::string file_name;
  std::string file_content, current_input;
  std::map<std::string, int> labels;

  std::vector<Instruction> instructions;

  // raise error with message, and exit the program with error code 1
  void raiseError(std::string msg) {
    std::cerr << msg << std::endl;
    exit(1);
  }

  // convert the hexadecimal value of register to the corresponding notation
  std::string bitToReg(uint16_t reg) {
    if (reg == 0x0) return "A";
    if (reg == 0x1) return "B";
    if (reg == 0x2) return "C";
    if (reg == 0x3) return "SR";
    if (reg == 0x4) return "HL";
    if (reg == 0x5) return "F";
    if (reg == 0x6) return "SP";
    if (reg == 0x7) return "PC";
    raiseError("Unknown Register Code " + file_name);
    return 0;
  }

  // convert the register notation to corresponding hexadecimal value
  uint8_t regToBit(std::string reg) {
    if (reg == "A") return 0x0;
    if (reg == "B") return 0x1;
    if (reg == "C") return 0x2;
    if (reg == "SR") return 0x3;
    if (reg == "HL") return 0x4;
    if (reg == "F") return 0x5;
    if (reg == "SP") return 0x6;
    if (reg == "PC") return 0x7;
    return 0xff;  // to catch errors;
  }

  // convert string to immediate
  uint8_t stringToImm8(std::string input) {
    // std::istringstream iss(v);
    // int intValue;
    // if (iss >> intValue) {
    //   if (intValue >= 0 && intValue <= 255) {
    //     uint8_t uint8Value = static_cast<uint8_t>(intValue);
    //     return uint8Value;
    //   } else {
    //     raiseError("Value is out of range for uint8_t");
    //   }
    // } else {
    //   raiseError("Conversion from string to integer failed");
    // }
    // return 1;
    try {
      size_t pos;
      uint8_t value = std::stoul(
          input, &pos, 0);  // The '0' parameter allows automatic base detection

      if (pos == input.length()) {
        return value;
      } else {
        raiseError("Conversion failed. Not a valid number.");
      }
    } catch (const std::invalid_argument& e) {
      raiseError("Invalid argument: ");
    } catch (const std::out_of_range& e) {
      raiseError("Out of range: ");
    }
    return 1;
  }

 public:
  AssemblyParser(const std::string& file_name);
  void parseFile();
  void skip();
  std::pair<std::string, std::string> getCleanOutput();
  void outputBinary(bool clean_file);
  ~AssemblyParser();
};