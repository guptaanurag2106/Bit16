#include "Bus.h"

#include "cpu.h"

Bus::Bus() {}
Bus::~Bus() {}

void Bus::write(uint16_t address, uint16_t value) {
  if (address >= 0 && address <= 0xffff) {
    ram[address] = value;
  } else {
    raiseError("Address: " + std::to_string(address) + " out of range");
  }
};

uint16_t Bus::read(uint16_t address) {
  if (address >= 0 && address <= 0xffff) {
    return ram[address];
  } else {
    raiseError("Address: " + std::to_string(address) + " out of range");
  }
  return 0;
};

void Bus::connectToCPU(CPU* c) { cpu = c; }