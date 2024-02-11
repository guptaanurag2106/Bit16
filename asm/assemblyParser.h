#pragma once

#include <getopt.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "../common/common.h"

const std::set<std::string> register_set = {"A", "B",  "C",  "D",
                                            "E", "SR", "HL", "F"};

class AssemblyParser {
 public:
  AssemblyParser(const std::string& file_name);
  void parseFile();
  void skip();
  void outputBinary(bool clean_file, int line_nums);
  std::pair<std::string, std::vector<uint16_t>> getCleanOutput(int line_nums);
  ~AssemblyParser();

 private:
  std::ifstream file;
  std::string file_name;
  std::string file_content, current_input;
  std::map<std::string, uint16_t> labels;
  std::map<std::string, uint8_t> constants;
  std::vector<Instruction> instructions;

  // convert the hexadecimal value of register to the corresponding notation
  std::string bitToReg(uint16_t reg) {
    if (reg == 0x0) return "A";
    if (reg == 0x1) return "B";
    if (reg == 0x2) return "C";
    if (reg == 0x3) return "D";
    if (reg == 0x4) return "E";
    if (reg == 0x5) return "SR";
    if (reg == 0x6) return "HL";
    if (reg == 0x7) return "F";
    raiseError("Unknown Register Code " + file_name);
    return 0;
  }

  // convert the register notation to corresponding hexadecimal value
  uint8_t regToBit(std::string reg) {
    if (reg == "A") return 0x0;
    if (reg == "B") return 0x1;
    if (reg == "C") return 0x2;
    if (reg == "D") return 0x3;
    if (reg == "E") return 0x4;
    if (reg == "SR") return 0x5;
    if (reg == "HL") return 0x6;
    if (reg == "F") return 0x7;
    return 0xff;  // to catch errors;
  }

  // convert string to immediate
  uint8_t stringToImm8(std::string input, const std::string message = "") {
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
        raiseError(message);
      }
    } catch (const std::invalid_argument& e) {
      raiseError(message);
    } catch (const std::out_of_range& e) {
      raiseError(message);
    }
    return 1;
  }

  uint16_t stringToImm16(std::string input, const std::string message = "") {
    try {
      size_t pos;
      uint16_t value = std::stoul(
          input, &pos, 0);  // The '0' parameter allows automatic base detection

      if (pos == input.length()) {
        return value;
      } else {
        raiseError(message);
      }
    } catch (const std::invalid_argument& e) {
      raiseError(message);
    } catch (const std::out_of_range& e) {
      raiseError(message);
    }
    return 1;
  }
};