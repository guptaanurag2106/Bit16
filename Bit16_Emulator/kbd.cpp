#include <iostream>

#include "Bus.h"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
// #include <conio.h>
#include <curses.h>
#endif
#include "kbd.h"

static int id = 2;
static std::string name = "Keyboard";
static int interrupt = 0;
static int interruptData = -1;
//   int cycles;

// Keyboard specific functions
static void keyboardTick(CPU& cpu) {
  std::cout << "Keyboard ticking..." << std::endl;
#ifdef _WIN32
  if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
    interrupt = 1;
    interruptData = 32;  // space
  } else if (GetAsyncKeyState(0x41) & 0x8000) {
    interrupt = 1;
    interruptData = 0x41;  // A

  } else if (GetAsyncKeyState(0x44) & 0x8000) {
    interrupt = 1;
    interruptData = 0x44;  // D

  } else if (GetAsyncKeyState(0x57) & 0x8000) {
    interrupt = 1;
    interruptData = 0x57;  // W

  } else if (GetAsyncKeyState(0x53) & 0x8000) {
    interrupt = 1;
    interruptData = 0x53;  // S
  }

#elif __linux__
  // keypad(stdscr, TRUE);
  // int ch = getch();
  // interrupt = 1;
  // interruptData = ch;
  // refresh();
#else
  std::cout << "Unknown OS" << std::endl;
#endif
}

static int keyboardSend(CPU& cpu) {
  cpu.write(KEYBOARD, interruptData);
  return interruptData;
}

static void keyboardReceive(CPU& cpu, int data) {
  std::cout << "Received data on the keyboard: " << data << std::endl;
}

static void keyboardDestroy(CPU& cpu) {
#ifdef __linux__
  // endwin();
#endif
  interrupt = 0;
  interruptData = -1;
  std::cout << "Keyboard destroyed." << std::endl;
}

Device* createKeyboardDevice() {
  // initscr();
  // cbreak();
  // noecho();
  // nodelay(stdscr, TRUE);
  return new Device{.id = id,
                    .name = name,
                    .interrupt = interrupt,
                    .interruptData = interruptData,
                    .cycles = 0,
                    .tick = keyboardTick,
                    .send = keyboardSend,
                    .receive = keyboardReceive,
                    .destroy = keyboardDestroy};
}
