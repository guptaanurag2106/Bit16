#include <getopt.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Bus.h"
#include "cpu.h"
#include "kbd.h"
#include "screen.h"

// Emulation cycle function
bool emulateCycle(CPU& cpu, std::vector<Device*>& devices, int& cycle) {
  for (auto& device : devices) {
    // Execute a portion of the device's logic during each cycle
    device->tick(cpu);

    // Check if the device triggered an interrupt
    if (device->interrupt) {
      // Handle interrupt
      std::cout << "Interrupt from " << device->name
                << "! Data: " << device->interruptData << std::endl;
      device->send(cpu);
      device->receive(cpu, -1);
      // Reset interrupt flag and data
      device->interrupt = 0;
      device->interruptData = -1;
    }

    // Increment the cycle counter for interleaved execution
    device->cycles++;
  }
  bool continue_emulation = cpu.run();
  return continue_emulation;
}

int main(int argc, char* argv[]) {
  std::string input_file_name;

  int load_address = 0;

  static struct option longOptions[] = {
      {"input-file", required_argument, 0, 'i'},
      {"load-address", required_argument, 0, 'l'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "i:l:h", longOptions, NULL)) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = std::string(optarg);
        if (input_file_name.substr(input_file_name.find_last_of('.') + 1) !=
            "bin") {
          std::cerr << "ROM file should be of type .bin" << std::endl;
          return 1;
        }
        break;
      case 'l':
        if (isdigit(optarg[0]) || (optarg[0] == '-' && isdigit(optarg[1]))) {
          load_address = std::atoi(optarg);  // Convert optarg to an integer
          if (load_address > ROM_END) {
            std::cerr << "Load address should be less than " << ROM_END
                      << std::endl;
            return 1;
          }
        } else {
          std::cerr << "Invalid integer value: " << optarg << std::endl;
          std::cerr << "Usage: -l, --load-address ADDRESS (INTEGER VALUE)"
                    << std::endl;
          return 1;
        }
        break;
      case 'h':
        // Display help menu
        std::cout << "Usage: " << argv[0] << "   [options] input_file(s)..."
                  << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -i, --input-file FILE    Specify input .bin ROM file"
                  << std::endl;
        std::cout
            << "  -l, --load-address  ADDRESS    Specify start address in ROM"
            << std::endl;
        std::cout << "  -h, --help               Display help message"
                  << std::endl;
        return 0;
      default:
        std::cerr << "Use '" << argv[0] << " --help' for usage." << std::endl;
        return 1;
    }
  }

  Bus bus = Bus();
  CPU cpu = CPU();

  std::cout << "Connecting Bus, CPU..." << std::endl;

  bus.connectToCPU(&cpu);
  cpu.connectToBus(&bus);

  cpu.setLoadingAddr(load_address);

  std::ifstream binaryFile(input_file_name, std::ios::in | std::ios::binary);

  if (binaryFile.is_open()) {
    binaryFile.read((char*)(bus.ram), 32768 * 2);
  }

  std::vector<Device*> devices;

  std::cout << "Adding devices..." << std::endl;
  devices.push_back(createKeyboardDevice());
  devices.push_back(createScreenDevice());

  if (devices.size() > cpu.MAX_DEVICES) {
    raiseError(std::to_string(cpu.MAX_DEVICES) + " Device limit exceeded");
  }

  std::cout << "Device List" << std::endl;
  for (auto& device : devices) {
    std::cout << device->id << " " << device->name << std::endl;
  }

  int cycle = 0;
  bool continue_emulation = true;

  while (cycle < 1000000000 && continue_emulation) {
    std::cout << "Emulation Cycle " << cycle + 1 << std::endl;
    continue_emulation = emulateCycle(cpu, devices, cycle);
  }

  for (auto& device : devices) {
    device->destroy(cpu);
  }

  return 0;
}
