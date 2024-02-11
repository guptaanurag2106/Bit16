#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
class Bus;
class CPU {
 public:
  const uint16_t MAX_DEVICES = 256;
  Bus* bus;
  void raiseError(std::string msg) {
    std::cerr << msg << std::endl;
    exit(1);
  }

  CPU();
  void setLoadingAddr(int);
  void dumpRegisters();
  bool run();
  void push(uint16_t);
  uint16_t pop();
  void print();
  void connectToBus(Bus*);
  void write(uint16_t, uint16_t);
  uint16_t read(uint16_t);
  ~CPU();

  void set_value(uint8_t reg, uint8_t value) {
    switch (reg) {
      case 0x0:
        A = value;
        break;
      case 0x1:
        B = value;
        break;
      case 0x2:
        C = value;
        break;
      case 0x3:
        D = value;
        break;
      case 0x4:
        E = value;
        break;
      case 0x5:
        HL = value;
        break;
      case 0x6:
        flag = value;
      default:
        break;
    }
  }

  uint16_t get_value(uint8_t reg) {
    switch (reg) {
      case 0x0:
        return A;
      case 0x1:
        return B;
      case 0x2:
        return C;
      case 0x3:
        return D;
      case 0x4:
        return E;
      case 0x5:
        return HL;
      case 0x6:
        return flag;
      default:
        return 0;
    }
  }

 private:
  uint16_t A, B, C, D, E, SR, HL, flag;
  uint16_t PC, SP;
  bool executeInstruction(uint16_t);
};

struct Device {
  int id;
  std::string name;
  int interrupt;
  int interruptData;
  int cycles;

  // Function pointers for device-specific operations
  void (*tick)(CPU&);
  int (*send)(CPU&);
  void (*receive)(CPU&, int);
  void (*destroy)(CPU&);
};
