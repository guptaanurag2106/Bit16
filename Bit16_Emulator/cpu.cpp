#include "cpu.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "Bus.h"

CPU::CPU() {}

void CPU::setLoadingAddr(int load_address) { PC = load_address; }

bool CPU::executeInstruction(uint16_t current_ins) {
  int opcode = current_ins >> 4;  // Extract the 4-bit opcode
  bool select = current_ins & 0x800;

  switch (opcode) {
    case 0x0:  // NOP
      PC++;
      break;
    case 0x1:  // HALT
      return false;
      break;
    case 0x2: {  // MW reg, reg/imm8
      uint8_t reg = current_ins & 0x700;
      if (select) {
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, imm8);
      } else {
        uint8_t reg2 = current_ins & 0xe0;
        set_value(reg, get_value(reg2));
      }
      PC++;
      break;
    }
    case 0x3: {  // MWL imm8
      uint8_t imm8 = current_ins & 0xFF;
      HL = (HL >> 8) << 8 + imm8;
      PC++;
      break;
    }
    case 0x4: {  // MWH imm8
      uint8_t imm8 = current_ins & 0xFF;
      HL = HL & 0xFF + imm8 << 8;
      PC++;
      break;
    }
    case 0x5: {  // LW reg, [HL/imm8]
      uint8_t reg = current_ins & 0x700;
      if (select) {
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(imm8));
      } else {
        uint8_t reg2 = current_ins & 0xe0;
        set_value(reg, bus->read(get_value(reg2)));
      }
      break;
    }
    case 0x6: {  // SW [HL/imm8], reg
      uint8_t reg = current_ins & 0xe0;
      if (select) {
        uint8_t imm8 = current_ins & 0xFF;
        bus->write(imm8, get_value(reg));
      } else {
        uint8_t reg2 = current_ins & 0x700;
        bus->write(reg2, get_value(reg2));
      }
      break;
    }
    case 0x7: {  // ADD reg, reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0x8: {  // SUB reg, reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0x9: {  // AND reg, reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0xa: {  // ADDC reg, reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0xb: {  // NOT reg, reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0xc: {  // JMPZ reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0xd: {  // JMPN reg/imm8
      if (select) {
      } else {
        uint8_t reg = current_ins & 0x700;
        uint8_t imm8 = current_ins & 0xFF;
        set_value(reg, bus->read(HL + imm8));
      }
      break;
    }
    case 0xe: {  // PUSH reg/imm8
      if (select) {
        uint8_t imm8 = current_ins & 0xFF;

        push(imm8);
      } else {
        uint8_t reg = current_ins & 0x700;
        push(bus->read(reg));
      }
      PC++;
      break;
    }
    case 0xf: {  // POP  reg
      uint8_t reg = current_ins & 0x700;
      uint16_t value = pop();
      set_value(reg, value);
      PC++;
      break;
    }
    default:
      std::cerr << "Unkown Instruction" << std::endl;
      dumpRegisters();
      exit(1);
  }
  return true;
}

bool CPU::run() {
  uint16_t current_instruction = bus->read(PC);
  bool continue_emulation = executeInstruction(current_instruction);
  return continue_emulation;
}

std::string hexstr(uint16_t n) {
  std::stringstream ss;
  ss << std::hex << std::setw(4) << std::setfill('0') << n;
  return ss.str();
}

void CPU::dumpRegisters() {
  std::cout << "A: " << hexstr(A) << " B: " << hexstr(B) << " C: " << hexstr(C)
            << " D: " << hexstr(D) << " E: " << hexstr(E)
            << " HL: " << hexstr(HL) << "\n";
  std::cout << "Flag: " << hexstr(flag) << "\n";
  std::cout << "PC: " << hexstr(PC) << "\n";
  std::cout << "SR: " << hexstr(SR) << std::endl;
}

void CPU::push(uint16_t value) { bus->write((1 << 16) | SP--, value); }

uint16_t CPU::pop() { return bus->read(((1 << 16) | SP++) + 1); }

void CPU::print() { std::cout << "asdfadsf" << std::endl; }

void CPU::connectToBus(Bus* bus1) { bus = bus1; }

void CPU::write(uint16_t address, uint16_t value) {
  bus->write(address, value);
}

uint16_t CPU::read(uint16_t address) { return bus->read(address); }

CPU::~CPU() { delete bus; }