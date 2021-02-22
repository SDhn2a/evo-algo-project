//  This file is part of My First Evolutionary Algorithm
//  Copyright (C) Matthew Andres Moreno, 2020.
//  Released under MIT license; see LICENSE
//  Modified by Soren DeHaan, 2021.

#include <iostream>

#include "emp/base/vector.hpp"
#include "emp/config/command_line.hpp"

#include "../evolve.h"

// This is the main function for the NATIVE version of My First Evolutionary Algorithm.

int main(int argc, char* argv[])
{
  emp::vector<std::string> args = emp::cl::args_to_strings(argc, argv);

  evolve();

  std::cout << "Evolution complete." << std::endl;
}
