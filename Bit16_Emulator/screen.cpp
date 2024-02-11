#include "screen.h"

#include <iostream>

#include "CPU.h"

static int id = 2;
static std::string name = "Screen";
static int interrupt = 0;
static int interruptData = -1;
//   int cycles;

// screen specific functions
static void screenTick(CPU& cpu) {
  std::cout << "screen ticking..." << std::endl;
  if (/* Some condition */ 1 == 1) {
    interrupt = 1;
    interruptData = 24;  // Example data
  }
}

static int screenSend(CPU& cpu) { return interruptData; }

static void screenReceive(CPU& cpu, int data) {
  std::cout << "Received data on the screen: " << data << std::endl;
}

static void screenDestroy(CPU& cpu) {
  interrupt = 0;
  interruptData = -1;
  std::cout << "screen destroyed." << std::endl;
}

Device* createScreenDevice() {
  return new Device{.id = id,
                    .name = name,
                    .interrupt = interrupt,
                    .interruptData = interruptData,
                    .cycles = 0,
                    .tick = screenTick,
                    .send = screenSend,
                    .receive = screenReceive,
                    .destroy = screenDestroy};
}
