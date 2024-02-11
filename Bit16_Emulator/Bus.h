#pragma once

#include <stdint.h>

#include "../common/common.h"
#include "cpu.h"

#define TOTAL_SIZE 65536
#define ROM_SIZE 32768
#define ROM_BEGIN 0x0
#define ROM_END 0x7fff
#define VRAM_SIZE 16384
#define VRAM_BEGIN 0x8000
#define VRAM_END 0xbfff
#define RAM_SIZE 15870
#define RAM_BEGIN 0xc000
#define RAM_END 0xfdfd

#define KEYBOARD 0xfdfe

class CPU;
class Bus {
 public:
  Bus();
  ~Bus();

  // Devices connected to the bus
  CPU* cpu;
  uint16_t ram[TOTAL_SIZE];

  void connectToCPU(CPU* cpu);

  void write(uint16_t address, uint16_t value);
  uint16_t read(uint16_t address);
};