//  This file is part of My First Evolutionary Algorithm
//  Copyright (C) Matthew Andres Moreno, 2020.
//  Released under MIT license; see LICENSE
//  Modified by Soren DeHaan, 2021.

#include <iostream>
#include <string>

#include "emp/base/vector.hpp"
#include "emp/config/command_line.hpp"
#include "emp/config/ArgManager.hpp"

#include "../evolve.h"

EMP_BUILD_CONFIG(MyConfigType,
    VALUE(SEED, int, 10, "What value should the random seed be?"), 
    VALUE(START_PA, double, 0.0, "What addition value should the starting organism have?"),
    VALUE(START_PM, double, 1.0, "What multiplication value should the starting organism have?"),
    VALUE(START_CA, double, 0.5, "What addition chance should the starting organism have?"),
    VALUE(START_CM, double, 0.5, "What multiplication chance should the starting organism have?"),
    VALUE(FILE_PATH, std::string, "", "Output file path"),
    VALUE(FILE_NAME, std::string, "_data.dat", "Root output file name")
)

int main(int argc, char* argv[])
{
  MyConfigType config;
  bool success = config.Read("MySettings.cfg");
  if(!success) config.Write("MySettings.cfg");

  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "SymSettings.cfg") == false) {
    std::cerr << "There was a problem in processing the options file." << std::endl;
    exit(1);
  }
  if (args.TestUnknown() == false) exit(0);

  // emp::vector<std::string> args = emp::cl::args_to_strings(argc, argv);

  evolve(config.SEED(),config.START_PA(),config.START_PM(),config.START_CA(),config.START_CM(),config.FILE_PATH(),config.FILE_NAME());

  std::cout << "Voila!" << std::endl;
}
