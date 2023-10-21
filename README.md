# Bit16

Bit16 is a 16-bit CPU, emulator, assembler, circuit with fully custom architecture

## Table of Contents

-   [Overview](#overview)
-   [Architecture Details](#architecture-details)
    -   [Register Set](#register-set)
    -   [Instruction Set](#instruction-set)
    -   [Memory Layout](#memory-layout)
-   [Getting Started](#getting-started)
    -   [Installation](#installation)

## Overview

The Bit16 project provides a framework for creating, testing, and experimenting with custom 16-bit CPU architectures. It includes the following components:

-   A 16-bit CPU emulator that can execute programs written in the custom assembly language.
-   An assembler for translating assembly code into machine code.
-   Example programs and projects to demonstrate the capabilities of the CPU.

## Architecture Details

The Bit16 CPU is a custom 16-bit architecture designed with the following specifications:

### Register Set

-   A (0x0): General Purpose Register
-   B (0x0): General Purpose Register
-   C (0x2): General Purpose Register
-   SR (0x3) Settings Register: left-to-right
    i/o, MB/(M)emory (B)ank (0x6) 0x0 for General Purpose RAM or 1 for VRAM

    i/o flag specifies usage of PORT or RAM for LW and SW instructions, can be used as a replacement for dedicated instructions like
    INB reg, red/immg : reg <- PORT[reg/imm8]
    OUTB reg/imm8, reg : PORT[reg/imm8] <- reg
    0 -> use current RAM
    1 -> use PORT

-   HL (0x4): HL 16-bit Register
-   Flag (0x5): Flag register with the following flags (left-to-right):
    -   Zero
    -   Negative
-   SP (0x6) Stack Pointer Register
-   PC (0x7) Program Counter Register

### Instruction Set

The Bit16 CPU supports the following instructions, each encoded with a 4-bit opcode:

| Opcode | Instruction       | Description                                                                          |
| ------ | ----------------- | ------------------------------------------------------------------------------------ |
| 0x0    | NOP               | No operation, increments the program counter                                         |
| 0x1    | HALT              | Stops the CPU, program execution is finished                                         |
| 0x2    | MW reg, reg/imm8  | Move data from a register or immediate value to a register                           |
| 0x3    | MWL imm8          | Move an immediate 8-bit value to the lower byte of HL                                |
| 0x4    | MWH imm8          | Move an immediate 8-bit value to the upper byte of HL                                |
| 0x5    | LW reg, [HL/imm8] | Load data from memory (RAM) to a register                                            |
| 0x6    | SW [HL/imm8], reg | Store data from a register to memory (RAM)                                           |
| 0x7    | ADD reg, reg/imm8 | Add data from a register or immediate value to a register                            |
| 0x8    | SUB reg, reg/imm8 | Subtract data from a register or immediate value from a register                     |
| 0x9    | AND reg, reg/imm8 | Perform a bitwise AND operation between a register and a register or immediate value |
| 0xA    | OR reg, reg/imm8  | Perform a bitwise OR operation between a register and a register or immediate value  |
| 0xB    | NOT reg, reg/imm8 | Perform a bitwise NOT operation on a register or immediate value                     |
| 0xC    | JMPZ reg/imm8     | Conditional jump based on the value in a register or immediate value                 |
| 0xD    | JMPN              | Conditional jump based on the Negative flag                                          |
| 0xE    | PUSH imm8/reg     | Push a value from a register or immediate value onto the stack                       |
| 0xF    | POP reg           | Pop a value from the stack into a register                                           |

### Memory Layout

The memory layout of the Bit16 CPU is as follows:

-   0x0000..0xFFFF: GENERAL PURPOSE ROM 65536\*16bit
-   0x0000..0x3FFF: GENERAL PURPOSE RAM (BANKED/VRAM) 16384\*16bit
-   0x0000..0xFDFF: GENERAL PURPOSE RAM 65024\*16bit
-   0xFE00..0xFFFF: STACK (RECOMMENDED), else GP RAM 512\*16bit

This memory layout provides a framework for storing program code, data, and stack information.

## Getting Started

### Installation

1. Clone this repository to your local machine.

```shell
git clone https://github.com/guptaanurag2106/Bit16.git
```

2. Build the emulator using your C++ compiler.

```shell
g++ -o Bit16 emulator.cpp
```
