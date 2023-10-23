#include <getopt.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>

#include "assemblyParser.h"

AssemblyParser::AssemblyParser(const std::string& file_name) {
  std::cout << "Processing " << file_name << "..." << std::endl;
  std::ifstream file(file_name, std::ios_base::binary | std::ios_base::in);
  if (!file.is_open()) {
    raiseError("Failed to open " + file_name);
  }
  using Iterator = std::istreambuf_iterator<char>;
  std::string content(Iterator{file}, Iterator{});
  if (!file) {
    raiseError("Failed to read " + file_name);
  }
  if (file.is_open()) file.close();
  file_content = content;
  current_input = content;
  int index = file_name.find('.');
  std::string temp = file_name.substr(0, index);
  this->file_name = temp;
}

// trim trailing and leading spaces
std::string trim(std::string s) {
  std::regex e("^\\s+|\\s+$");
  return std::regex_replace(s, e, "");
}

// main parser
void AssemblyParser::parseFile() {
  skip();
  std::vector<std::string> lines;
  std::istringstream input_stream(current_input);
  std::string line;

  int lines_removed = std::ranges::count(file_content, '\n') -
                      std::ranges::count(current_input, '\n') + 1;

  int label_index = 0;
  int line_index = lines_removed;
  std::vector<int> jmp_indices;
  while (std::getline(input_stream, line, '\n')) {
    line = trim(line);
    if (line.size() > 0) {
      if (line[0] == '@') {
        lines.push_back("<TEMPORARY_CHAR>");
        lines.push_back(line);
        label_index += 2;
        line_index++;
        instructions.push_back(Instruction());
        instructions.push_back(Instruction());
        jmp_indices.push_back(label_index);
        continue;
      } else if (line[0] == '.' & line[line.size() - 1] == ':') {
        int space = line.find(' ');
        labels[line.substr(1, space)] = label_index;
        line_index++;

        continue;
      }
      std::vector<std::string> instruction_line;
      std::regex space_regex("^\\s+");
      line = std::regex_replace(line, space_regex, " ");
      std::string s = "";
      for (int i = 0; i < line.size(); i++) {
        if (line[i] != ' ') {
          s += line[i];
        } else {
          instruction_line.push_back(s);
          s = "";
        }
      }
      if (s != "") instruction_line.push_back(s);
      std::string mnemonic = instruction_line[0];
      if (instruction_set.find(mnemonic) == instruction_set.end()) {
        raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                   +" Unknown Instruction: " + mnemonic);
      }
      InstructionType type = instruction_set.at(mnemonic);
      uint8_t opcode = opcode_set.at(mnemonic);
      if (type == NoParams) {
        if (instruction_line.size() != 1) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     mnemonic + " takes no params, found: " +
                     std::to_string(instruction_line.size() - 1));
        }
        instructions.push_back(Instruction(mnemonic, opcode, type));
      } else if (type == Register_only) {
        if (instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     mnemonic + " takes 1 param (register), found: " +
                     std::to_string(instruction_line.size() - 1));
        }

        uint8_t bit = regToBit(instruction_line[1]);
        if (bit == 0xff) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     mnemonic + " takes 1 param (register), found: " +
                     instruction_line[1]);
        }
        instructions.push_back(Instruction(mnemonic, opcode, type,
                                           InstructionParams(bit, 0, 0),
                                           InstructionParams()));
      } else if (type == Immediate_only) {
        if (instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     mnemonic + " takes 1 param (8 bit immediate), found: " +
                     std::to_string(instruction_line.size() - 1));
        }
        uint8_t imm8 = stringToImm8(instruction_line[1]);
        instructions.push_back(Instruction(mnemonic, opcode, type,
                                           InstructionParams(0, imm8, 1),
                                           InstructionParams()));
      } else if (type == Register_Immediate_only) {
        if (instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     mnemonic +
                     " takes 1 param (register/8 bit immediate), found: " +
                     std::to_string(instruction_line.size() - 1));
        }
        if (register_set.contains(instruction_line[1])) {
          uint8_t bit = regToBit(instruction_line[1]);
          if (bit == 0xff) {
            raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                       mnemonic +
                       " takes 1 param (register/8 bit immediate), found: " +
                       instruction_line[1]);
          }
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit, 0, 0),
                                             InstructionParams()));

        } else {
          uint8_t imm8 = stringToImm8(instruction_line[1]);
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(0, imm8, 1),
                                             InstructionParams()));
        }
      } else if (type == ALL_1) {
        if (instruction_line.size() != 3) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) +
              mnemonic +
              " takes 2 param (register, register/8 bit immediate), found: " +
              std::to_string(instruction_line.size() - 1));
        } else if (instruction_line[1][instruction_line[1].size() - 1] != ',') {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     "expected ','");
        }
        uint8_t bit = regToBit(
            instruction_line[1].substr(0, instruction_line[1].size() - 1));
        if (bit == 0xff) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) +
              mnemonic +
              " takes 2 param (register, register/8 bit immediate), found: " +
              instruction_line[1]);
        }
        if (register_set.contains(instruction_line[2])) {
          uint8_t bit1 = regToBit(instruction_line[2]);
          if (bit == 0xff) {
            raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                       mnemonic +
                       " takes 2 param (register, register/8 bit immediate), "
                       "found: " +
                       instruction_line[2]);
          }
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit, 0, 0),
                                             InstructionParams(bit1, 0, 0)));

        } else {
          uint8_t imm8 = stringToImm8(instruction_line[2]);
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit, 0, 0),
                                             InstructionParams(0, imm8, 1)));
        }

      } else {
        if (instruction_line.size() != 3) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) +
              mnemonic +
              " takes 2 param (register, register/8 bit immediate), found: " +
              std::to_string(instruction_line.size() - 1));
        } else if (instruction_line[1][instruction_line[1].size() - 1] != ',') {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     "expected ','");
        }
        uint8_t bit = regToBit(instruction_line[2]);
        if (bit == 0xff) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) +
              mnemonic +
              " takes 2 param (register/8 bit immediate, register), found: " +
              instruction_line[2]);
        }
        if (register_set.contains(instruction_line[1].substr(
                0, instruction_line[1].size() - 1))) {
          uint8_t bit1 = regToBit(
              instruction_line[1].substr(0, instruction_line[1].size() - 1));
          if (bit == 0xff) {
            raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                       mnemonic +
                       " takes 2 param (register/8 bit immediate, register), "
                       "found: " +
                       instruction_line[1]);
          }
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit1, 0, 0),
                                             InstructionParams(bit, 0, 0)));

        } else {
          uint8_t imm8 = stringToImm8(instruction_line[1]);
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(imm8, 0, 1),
                                             InstructionParams(bit, 0, 0)));
        }
      }
      lines.push_back(line);
      label_index++;
    }
    line_index++;
  }

  for (auto i : instructions) {
    std::cout << i << "\n";
  }

  for (auto i : labels) {
    std::cout << i.first << " " << i.second << "\n";
  }
  // TODO: handle binary/hexadecimal values
  // !:replace empty instructions for JUMP with MWL and MWH
  // TODO: handle import export
  // !: handle variable definitions
}

// Remove over whitespace and comments
void AssemblyParser::skip() {
  std::regex space_regex("^\\s+");
  std::regex empty_line_regex("^[ \\t]*$");
  std::regex comment_regex("//[^\n]*");
  std::regex mult_comments_regex("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/");

  current_input = std::regex_replace(current_input, empty_line_regex, "");
  current_input = std::regex_replace(current_input, space_regex, "");
  current_input = std::regex_replace(current_input, comment_regex, "");
  current_input = std::regex_replace(current_input, mult_comments_regex, "");
  // std::cout << current_input;

  // std::smatch match;
  // if (std::regex_search(current_input, match, space_regex)) {
  //   // If the regex pattern is found at the beginning of the string
  //   std::string matched_string = match.str();
  //   std::cout << "Matched: <" << matched_string << ">" << std::endl;

  //   // Remove the matched part from the input string
  //   current_input = current_input.substr(matched_string.length());
  // }
  // if (std::regex_search(current_input, match, comment_regex)) {
  //   std::string matched_string = match.str();
  //   std::cout << "Matched1: <" << matched_string << ">" << std::endl;

  //   current_input = current_input.substr(matched_string.length());
  // }
  // if (std::regex_search(current_input, match, mult_comments_regex)) {
  //   std::string matched_string = match.str();
  //   std::cout << "Matched2: <" << matched_string << ">" << std::endl;

  //   current_input = current_input.substr(matched_string.length());
  // }
}

std::pair<std::string, std::string> AssemblyParser::getCleanOutput() {
  std::string output = "";
  std::string bin = "";
  for (auto i : instructions) {
    std::string mnemonic = i.mnemonic;
    uint8_t opcode = i.opcode;
    InstructionType type = i.type;
    InstructionParams param1 = i.param1;
    InstructionParams param2 = i.param2;
    std::string current;
    std::string current_bin;

    if (type == NoParams) {
      current = mnemonic;
      current_bin = std::bitset<4>(opcode).to_string() + "000000000000";
    } else if (type == Register_only) {
      current = mnemonic + " " + bitToReg(param1.reg);
      current_bin = std::bitset<4>(opcode).to_string() + "0" +
                    std::bitset<3>(param1.reg).to_string() + "00000000";
    } else if (type == Immediate_only) {
      current = mnemonic + " " + std::to_string(param1.imm8);
      current_bin = std::bitset<4>(opcode).to_string() + "0000" +
                    std::bitset<8>(param1.imm8).to_string();
    } else if (type == Register_Immediate_only) {
      if (!param1.choice) {
        current = mnemonic + " " + bitToReg(param1.reg);
        current_bin = std::bitset<4>(opcode).to_string() + "0" +
                      std::bitset<3>(param1.reg).to_string() + "00000000";
      } else {
        current = mnemonic + " " + std::to_string(param1.imm8);
        current_bin = std::bitset<4>(opcode).to_string() + "1000" +
                      std::bitset<8>(param1.imm8).to_string();
      }

    } else if (type == ALL_1) {
      if (!param2.choice) {
        current =
            mnemonic + " " + bitToReg(param1.reg) + "," + bitToReg(param2.reg);

        current_bin = std::bitset<4>(opcode).to_string() + "0" +
                      std::bitset<3>(param1.reg).to_string() +
                      std::bitset<3>(param2.reg).to_string() + "00000";
      } else {
        current = mnemonic + " " + bitToReg(param1.reg) + "," +
                  std::to_string(param2.imm8);
        current_bin = std::bitset<4>(opcode).to_string() + "0" +
                      std::bitset<3>(param1.reg).to_string() +
                      std::bitset<8>(param2.imm8).to_string();
      }
    } else if (type == ALL_SW) {
      if (!param1.choice) {
        current =
            mnemonic + " " + bitToReg(param1.reg) + "," + bitToReg(param2.reg);

        current_bin = std::bitset<4>(opcode).to_string() + "0" +
                      std::bitset<3>(param1.reg).to_string() +
                      std::bitset<3>(param2.reg).to_string() + "00000";
      } else {
        current = mnemonic + " " + std::to_string(param1.imm8) + "," +
                  bitToReg(param2.reg);
        current_bin = std::bitset<4>(opcode).to_string() + "0" +
                      std::bitset<3>(param1.reg).to_string() +
                      std::bitset<8>(param2.imm8).to_string();
      }
    } else {
      raiseError(mnemonic + " can't be parsed " + file_name);
    }

    output += current;
    output += "\n";
    bin += current_bin;
    bin += "\n";
  }

  return std::make_pair(output, bin);
}

void AssemblyParser::outputBinary(bool clean_file) {
  std::pair<std::string, std::string> output = getCleanOutput();
  if (clean_file) {
    std::fstream cleanfile(file_name + "_clean.txt", std::fstream::out);
    cleanfile << output.first;
    cleanfile.close();
  }
  std::fstream binaryfile(file_name + ".bin", std::fstream::out);
  binaryfile << output.second;
  binaryfile.close();
}

AssemblyParser::~AssemblyParser() {
  if (file.is_open()) file.close();
  file_content.clear();
  current_input.clear();
}

int main(int argc, char* argv[]) {
  int option;
  bool clean_file = false;
  std::vector<std::string> input_file_names;
  while ((option = getopt(argc, argv, "hi:")) != -1) {
    switch (option) {
      case 'h':
        // Display help message
        std::cout << "Usage: asm [-h] [-o] [-i <input_file>]\n";
        std::cout << "positional arguments:\n";
        std::cout << "options:\n";
        std::cout << "-i <input_file>,  input files to assemble\n";
        std::cout << "-o,               output clean assembly file\n";
        std::cout << "-h,               show help message" << std::endl;
        break;
      case 'i':
        // Assemble the specified input file
        input_file_names.push_back(optarg);
        break;
      // case 'o':
      //   clean_file = true;
      //   break;
      default:
        std::cerr << "Invalid option. Use -h for help.\n";
        return 1;
    }
  }

  for (std::string file_name : input_file_names) {
    AssemblyParser parser(file_name);
    parser.parseFile();
    std::cout << clean_file << "asdfasdfasdf\n";
    parser.outputBinary(true);
  }

  return 0;
}