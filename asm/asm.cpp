// TODO: handle binary/hexadecimal values
// TODO: handle import export
// !: handle variable definitions
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
  std::regex pattern(" ,");

  s = std::regex_replace(s, e, "");
  s = std::regex_replace(s, pattern, ",");
  return s;
}

// main parser
void AssemblyParser::parseFile() {
  skip();
  std::vector<std::string> lines;
  std::istringstream input_stream(current_input);
  std::string line;

  int label_index = 0;
  int line_index = 0;
  while (std::getline(input_stream, line, '\n')) {
    line_index++;
    line = trim(line);
    if (line.size() > 0) {
      if (line[line.size() - 1] == ':') {
        labels[line.substr(0, line.length() - 1)] = label_index;
      } else if (line[0] == '.') {
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

        if (instruction_line[0] != ".org" || instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     +" Unknown Directive: " + line);
        }
        uint16_t address = stringToImm16(
            instruction_line[1],
            file_name + ".asm Line: " + std::to_string(line_index) +
                +" Invalid Address: " + instruction_line[1]);
        label_index = address;

      } else if (line[0] == '@') {
        label_index += 2;
      } else if (line.substr(0, 4) == "const") {
        std::vector<std::string> instruction_line;
        std::string s;
        for (int i = 0; i < line.size(); i++) {
          if (line[i] != ' ') {
            s += line[i];
          } else {
            instruction_line.push_back(s);
            s = "";
          }
        }
        if (s != "") instruction_line.push_back(s);
        if (instruction_line[0] != "const" || instruction_line.size() != 3)
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     +" Unknown Syntax: " + line);

        constants[instruction_line[1]] = stringToImm8(
            instruction_line[2],
            file_name + ".asm Line: " + std::to_string(line_index) +
                +" Invalid Constant: " + instruction_line[2]);

        label_index += 1;
      } else {
        label_index += 1;
      }
    }
  }
  input_stream.clear();
  input_stream.seekg(0);
  line_index = 0;

  while (std::getline(input_stream, line, '\n')) {
    line_index++;
    line = trim(line);
    if (line.size() > 0) {
      if (line[0] == '@') {
        std::string label = line.substr(1);
        if (labels.find(label) == labels.end()) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     +" Unknown Label: " + label);
        }
        label_index = labels[label];
        uint8_t msb = static_cast<uint8_t>(label_index >> 8);

        uint8_t lsb = static_cast<uint8_t>(label_index & 0xFF);
        InstructionType type = instruction_set.at("MWH");
        uint8_t opcode = opcode_set.at("MWH");

        InstructionType type1 = instruction_set.at("MWL");
        uint8_t opcode1 = opcode_set.at("MWL");

        instructions.push_back(Instruction("MWH", opcode, type,
                                           InstructionParams(0, msb, 1),
                                           InstructionParams()));

        instructions.push_back(Instruction("MWL", opcode1, type1,
                                           InstructionParams(0, lsb, 1),
                                           InstructionParams()));

        lines.push_back(line);
        continue;
      } else if (line[line.size() - 1] == ':') {
        continue;
      } else if (line[0] == '.') {
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

        if (instruction_line[0] != ".org" || instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     +" Unknown Directive: " + line);
        }
        uint16_t address;
        if (constants.find(instruction_line[1]) != constants.end()) {
          address = constants[instruction_line[1]];
        } else {
          address = stringToImm16(
              instruction_line[1],
              file_name + ".asm Line: " + std::to_string(line_index) +
                  +" Invalid Address: " + instruction_line[1]);
        }
        int current_ins = instructions.size();
        if (address < current_ins) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     +" Address: " + std::to_string(address) +
                     " is less than current number of instructions: " +
                     std::to_string(current_ins));
        }
        for (int i = 0; i < (address - current_ins); i++) {
          instructions.push_back(Instruction("NOP", opcode_set.at("NOP"),
                                             instruction_set.at("NOP")));
        }

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
          if (constants.find(s) != constants.end()) {
            s = std::to_string(constants[s]);
          }
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
                     " " + mnemonic + " takes no params, found: " +
                     std::to_string(instruction_line.size() - 1));
        }
        instructions.push_back(Instruction(mnemonic, opcode, type));
      } else if (type == Register_only) {
        if (instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     " " + mnemonic + " takes 1 param (register), found: " +
                     std::to_string(instruction_line.size() - 1));
        }

        uint8_t bit = regToBit(instruction_line[1]);
        if (bit == 0xff) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     " " + mnemonic + " takes 1 param (register), found: " +
                     instruction_line[1]);
        }
        instructions.push_back(Instruction(mnemonic, opcode, type,
                                           InstructionParams(bit, 0, 0),
                                           InstructionParams()));
      } else if (type == Immediate_only) {
        if (instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     " " + mnemonic +
                     " takes 1 param (8 bit immediate), found: " +
                     std::to_string(instruction_line.size() - 1));
        }
        uint8_t imm8 = stringToImm8(
            instruction_line[1],
            file_name + ".asm Line: " + std::to_string(line_index) + " " +
                mnemonic + " takes 1 param (8 bit immediate), found: " +
                instruction_line[1]);
        instructions.push_back(Instruction(mnemonic, opcode, type,
                                           InstructionParams(0, imm8, 1),
                                           InstructionParams()));
      } else if (type == Register_Immediate_only) {
        if (instruction_line.size() != 2) {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     " " + mnemonic +
                     " takes 1 param (register/8 bit immediate), found: " +
                     std::to_string(instruction_line.size() - 1));
        }
        if (register_set.contains(instruction_line[1])) {
          uint8_t bit = regToBit(instruction_line[1]);
          if (bit == 0xff) {
            raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                       " " + mnemonic +
                       " takes 1 param (register/8 bit immediate), found: " +
                       instruction_line[1]);
          }
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit, 0, 0),
                                             InstructionParams()));

        } else {
          uint8_t imm8 = stringToImm8(
              instruction_line[1],
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
                  mnemonic +
                  " takes 1 param (register/8 bit immediate), found: " +
                  instruction_line[1]);
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(0, imm8, 1),
                                             InstructionParams()));
        }
      } else if (type == ALL_1) {
        if (instruction_line.size() != 3) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
              mnemonic +
              " takes 2 param (register, register/8 bit immediate), found: " +
              std::to_string(instruction_line.size() - 1));
        }

        if (instruction_line[1][instruction_line[1].size() - 1] != ',') {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     " " + "expected ','");
        }
        uint8_t bit = regToBit(
            instruction_line[1].substr(0, instruction_line[1].size() - 1));
        if (bit == 0xff) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
              mnemonic +
              " takes 2 param (register, register/8 bit immediate), found: " +
              instruction_line[1]);
        }
        if (register_set.contains(instruction_line[2])) {
          uint8_t bit1 = regToBit(instruction_line[2]);
          if (bit == 0xff) {
            raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                       " " + mnemonic +
                       " takes 2 param (register, register/8 bit immediate), "
                       "found: " +
                       instruction_line[2]);
          }
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit, 0, 0),
                                             InstructionParams(bit1, 0, 0)));

        } else {
          uint8_t imm8 = stringToImm8(
              instruction_line[2],
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
                  mnemonic +
                  " takes 2 param (register, register/8 bit immediate), "
                  "found: " +
                  instruction_line[2]);
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit, 0, 0),
                                             InstructionParams(0, imm8, 1)));
        }

      } else {
        if (instruction_line.size() != 3) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
              mnemonic +
              " takes 2 param (register/8 bit immediate, register), found: " +
              std::to_string(instruction_line.size() - 1));
        } else if (instruction_line[1][instruction_line[1].size() - 1] != ',') {
          raiseError(file_name + ".asm Line: " + std::to_string(line_index) +
                     " " + "expected ','");
        }
        uint8_t bit = regToBit(instruction_line[2]);
        if (bit == 0xff) {
          raiseError(
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
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
                       " " + mnemonic +
                       " takes 2 param (register/8 bit immediate, register), "
                       "found: " +
                       instruction_line[1]);
          }
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(bit1, 0, 0),
                                             InstructionParams(bit, 0, 0)));

        } else {
          uint8_t imm8 = stringToImm8(
              instruction_line[1],
              file_name + ".asm Line: " + std::to_string(line_index) + " " +
                  mnemonic +
                  " takes 2 param (register/8 bit immediate, register), "
                  "found: " +
                  instruction_line[1]);
          instructions.push_back(Instruction(mnemonic, opcode, type,
                                             InstructionParams(0, imm8, 1),
                                             InstructionParams(bit, 0, 0)));
        }
      }
      lines.push_back(line);
    }
  }

  // for (auto i : instructions) {
  //   std::cout << i << "\n";
  // }

  // for (auto i : labels) {
  //   std::cout << i.first << " " << i.second << "\n";
  // }
}

// Remove over whitespace and comments
void AssemblyParser::skip() {
  // std::regex space_regex("\\s+");
  // std::regex empty_line_regex("^[ \\t]*$");
  std::regex comment_regex(";[^\n]*");

  // current_input = std::regex_replace(current_input, space_regex, "");
  // current_input = std::regex_replace(current_input, empty_line_regex, "");
  current_input = std::regex_replace(current_input, comment_regex, "");
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

std::pair<std::string, std::vector<uint16_t>> AssemblyParser::getCleanOutput(
    int line_nums) {
  std::string output = "";
  std::vector<uint16_t> bin;
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
    std::bitset<16> bitset(current_bin);
    uint16_t result = static_cast<uint16_t>(bitset.to_ulong());
    bin.push_back(result);
  }

  if (line_nums != -2) {
    int extra = line_nums * 1024.0 * 8 / 16.0 - instructions.size();
    if (extra < 0)
      raiseError("Specified file size" + std::to_string(line_nums) +
                 "kiB is smaller than program size " +
                 std::to_string(instructions.size() * 16 / 8192) + "kiB");
    for (int i = 0; i < extra; i++) {
      std::bitset<16> bitset("0000000000000000");
      uint16_t result = static_cast<uint16_t>(bitset.to_ulong());
      bin.push_back(result);
      output += "NOP\n";
    }
  }

  return std::make_pair(output, bin);
}

void AssemblyParser::outputBinary(bool clean_file, int line_nums) {
  std::pair<std::string, std::vector<uint16_t>> output =
      getCleanOutput(line_nums);
  if (clean_file) {
    std::fstream cleanfile(file_name + "_clean.txt", std::fstream::out);
    if (cleanfile.is_open()) {
      cleanfile << output.first;
    } else {
      raiseError("Error opening file: " + file_name + "_clean.txt");
    }
    cleanfile.close();
  }
  std::ofstream binaryfile(file_name + ".bin", std::ios::binary);
  if (binaryfile.is_open()) {
    binaryfile.write(reinterpret_cast<const char*>(output.second.data()),
                     output.second.size());
    binaryfile.close();
  } else {
    raiseError("Error opening file: " + file_name + ".bin");
  }
}

AssemblyParser::~AssemblyParser() {
  if (file.is_open()) file.close();
  file_content.clear();
  current_input.clear();
}

int main(int argc, char* argv[]) {
  // int option;
  std::vector<std::string> input_file_names;

  bool outputToTerminal = false;
  bool outputCleanFile = false;
  int line_nums = -2;

  static struct option longOptions[] = {
      {"input-file", required_argument, 0, 'i'},
      {"bin-size", required_argument, 0, 's'},
      {"output-terminal", no_argument, 0, 'o'},
      {"output-clean", no_argument, 0, 'c'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "i:s:och", longOptions, NULL)) != -1) {
    switch (opt) {
      case 'i':
        input_file_names.push_back(optarg);
        break;
      case 's':
        if (isdigit(optarg[0]) || (optarg[0] == '-' && isdigit(optarg[1]))) {
          line_nums = std::atoi(optarg);  // Convert optarg to an integer
        } else {
          std::cerr << "Invalid integer value: " << optarg << std::endl;
          std::cerr << "Usage: -s SIZE (INTEGER VALUE)" << std::endl;
          return 1;
        }
        break;
      case 'o':
        outputToTerminal = true;
        break;
      case 'c':
        outputCleanFile = true;
        break;
      case 'h':
        // Display help menu
        std::cout << "Usage: " << argv[0] << "  [options] input_file(s)..."
                  << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -i, --input-file FILE   Specify input .asm file(s) "
                     "(multiple allowed)"
                  << std::endl;
        std::cout
            << "  -s, --bin-size  SIZE    Specify output binary size in kiB"
            << std::endl;
        std::cout << "  -o, --output-terminal   Output to the terminal"
                  << std::endl;
        std::cout << "  -c, --output-clean      Output clean file" << std::endl;
        std::cout << "  -h, --help              Display help message"
                  << std::endl;
        return 0;
      default:
        std::cerr << "Use '" << argv[0] << " --help' for usage." << std::endl;
        return 1;
    }
  }

  if (input_file_names.empty()) {
    std::cerr << "At least one input file name is required." << std::endl;
    return 1;
  }

  for (const std::string& file_name : input_file_names) {
    AssemblyParser parser(file_name);
    parser.parseFile();
    parser.outputBinary(outputCleanFile, line_nums);
  }

  return 0;
}