#pragma once
#include <getopt.h>

#include <iostream>
#include <map>

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

// cout format for InstructionParam
inline std::ostream& operator<<(std::ostream& os,
                                const InstructionParams& param) {
  os << param.choice ? param.imm8 : param.reg;
  return os;
}

struct Instruction {
  std::string mnemonic;
  uint8_t opcode;
  InstructionType type;
  InstructionParams param1;
  InstructionParams param2;
  Instruction(std::string m, uint8_t o, InstructionType t)
      : mnemonic(m), opcode(o), type(t) {}

  Instruction(std::string m, uint8_t o, InstructionType t, InstructionParams p1,
              InstructionParams p2)
      : mnemonic(m), opcode(o), type(t), param1(p1), param2(p2) {}
};

// cout format for Instrucion
inline std::ostream& operator<<(std::ostream& os, const Instruction& ins) {
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
    {"ADDC", ALL_1},
    {"NOT", ALL_1},
    {"JMPZ", Register_Immediate_only},
    {"JMPN", Register_Immediate_only},
    {"PUSH", Register_Immediate_only},
    {"POP", Register_only},
};
const std::map<std::string, uint8_t> opcode_set = {
    {"NOP", 0x0},  {"HALT", 0x1}, {"MW", 0x2},   {"MWL", 0x3},
    {"MWH", 0x4},  {"LW", 0x5},   {"SW", 0x6},   {"ADD", 0x7},
    {"SUB", 0x8},  {"AND", 0x9},  {"ADDC", 0xa}, {"NOT", 0xb},
    {"JMPZ", 0xc}, {"JMPN", 0xd}, {"PUSH", 0xe}, {"POP", 0xf},
};

inline void raiseError(std::string msg) {
  std::cerr << msg << std::endl;
  exit(1);
}