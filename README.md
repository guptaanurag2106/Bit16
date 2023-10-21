# Bit16

Bit16 is a 16-bit CPU, emulator, assembler, circuit with fully custom architecture 

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture Details](#architecture-details)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [Running the Emulator](#running-the-emulator)
  - [Assembling Programs](#assembling-programs)
- [Assembly Language](#assembly-language)
- [Contributing](#contributing)
- [License](#license)

## Overview

The Bit16 project provides a framework for creating, testing, and experimenting with custom 16-bit CPU architectures. It includes the following components:

- A 16-bit CPU emulator that can execute programs written in the custom assembly language.
- An assembler for translating assembly code into machine code.
- Example programs and projects to demonstrate the capabilities of the CPU.

## Features

- 16-bit CPU emulation with support for custom instructions.
- Assembly language for programming the CPU.
- Basic I/O handling through ports and RAM.
- Example programs and projects.
- Extensible architecture for experimentation.

## Architecture Details

The Bit16 CPU is a custom 16-bit architecture designed with the following specifications:

### Register Set

- A (0x0): General Purpose Register
- B (0x0): General Purpose Register
- C (0x2): General Purpose Register
- D (0x3): General Purpose Register
- HL (0x4): HL 16-bit Register

### Flag Register

- Flag (0x5): Flag register with the following flags (left-to-right):
  - Zero
  - Negative
  - I/O (specifies usage of PORT or RAM for LW and SW instructions)

### Instruction Set

The Bit16 CPU supports the following instructions, each encoded with a 4-bit opcode:

| Opcode | Instruction        | Description                                      |
| ------ | ------------------ | ------------------------------------------------ |
| 0x0    | NOP                | No operation, increments the program counter    |
| 0x1    | HALT               | Stops the CPU, program execution is finished    |
| 0x2    | MW reg, reg/imm8   | Move data from a register or immediate value to a register |
| 0x3    | MWL imm8           | Move an immediate 8-bit value to the lower byte of HL |
| 0x4    | MWH imm8           | Move an immediate 8-bit value to the upper byte of HL |
| 0x5    | LW reg, [HL/imm8]  | Load data from memory (RAM) to a register        |
| 0x6    | SW [HL/imm8], reg  | Store data from a register to memory (RAM)       |
| 0x7    | ADD reg, reg/imm8  | Add data from a register or immediate value to a register |
| 0x8    | SUB reg, reg/imm8  | Subtract data from a register or immediate value from a register |
| 0x9    | AND reg, reg/imm8  | Perform a bitwise AND operation between a register and a register or immediate value |
| 0xA    | OR reg, reg/imm8   | Perform a bitwise OR operation between a register and a register or immediate value |
| 0xB    | NOT reg, reg/imm8  | Perform a bitwise NOT operation on a register or immediate value |
| 0xC    | JMPZ reg/imm8      | Conditional jump based on the value in a register or immediate value |
| 0xD    | JMPN               | Conditional jump based on the Negative flag     |
| 0xE    | PUSH imm8/reg      | Push a value from a register or immediate value onto the stack |
| 0xF    | POP reg            | Pop a value from the stack into a register       |

### Memory Layout

The memory layout of the Bit16 CPU is as follows:

- 0x0000..0x7FFF: General Purpose ROM (32KB)
- 0x8000..0xBFFF: General Purpose RAM (BANKED/VRAM) (16KB)
- 0xC000..0xFDFF: General Purpose RAM (15.9KB)
- 0xFE00..0xFEFF: Stack (256B) or GP RAM (if not used for stack)
- 0xFF00..0xFFF9: General Purpose RAM (10B)
- 0xFFFA..0xFFFB: Memory Bank (16 bits, 0x0 or 0x4000)
- 0xFFFC..0xFFFD: Stack Pointer (SP)
- 0xFFFE..0xFFFF: Program Counter (PC)

This memory layout provides a framework for storing program code, data, and stack information.

## Getting Started

### Prerequisites

- C++ compiler (e.g., g++) for building the emulator.
- An understanding of your custom CPU architecture (instruction set, registers, memory layout).

### Installation

1. Clone this repository to your local machine.

```shell
git clone https://github.com/yourusername/Bit16.git
```
2. Build the emulator using your C++ compiler.

```shell
g++ -o Bit16 emulator.cpp
```
