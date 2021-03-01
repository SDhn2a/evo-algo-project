//  This file is part of My First Evolutionary Algorithm
//  Copyright (C) Matthew Andres Moreno, 2020.
//  Released under MIT license; see LICENSE
//  Modified by Soren DeHaan, 2021.

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <pthread.h>

#include "emp/base/vector.hpp"
#include "emp/config/command_line.hpp"
#include "emp/config/ArgManager.hpp"

#include "../evolve.h"
#include "../DataCenter.h"

EMP_BUILD_CONFIG(MyConfigType,
    VALUE(SEED, int, 10, "What value should the random seed be?"),
    VALUE(SEED_RANGE, size_t, 3, "How many seeds should each thread run?"),
    VALUE(THREAD_CT, size_t, 10, "How many threads should be run?"),
    VALUE(WIDE_TEST, size_t, 20, "How many random tests do we run?"),
    VALUE(POP_SIZE, size_t, 50, "How many organisms per generation?"),
    VALUE(GENS, size_t, 600, "How many generations?"),
    VALUE(FILE_PATH, std::string, "", "Output file path"),
    VALUE(FILE_NAME, std::string, "_data.dat", "Root output file name"),
    VALUE(FILE_FILE, std::string, "metafile.dat", "Link to data file list"),
    VALUE(FILE_CNCT, std::string, "connect.dat", "Link to connection file")
)
  
struct threadFeed{
  int seedStart;
  int seedEnd;
  size_t popSize;
  size_t gens;
  std::string filePath;
  std::string fileName;
  std::string fileFile;
  DataCenter dc;
};

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

  DataCenter dataCenter;
  dataCenter.importData(config.FILE_FILE());
  dataCenter.standardizeDataByFile();
  dataCenter.sampleData();
  dataCenter.makeConnectivity(config.FILE_CNCT());
  for(size_t i = 9973; i < 9973+config.WIDE_TEST(); i++){
    dataCenter.dataToIOData(i);
  }
  
  std::vector<std::thread> threadList;
  for (size_t i = 0; i < config.THREAD_CT(); i++) {
    threadFeed tf;
    tf.seedStart = config.SEED()+i*config.SEED_RANGE();
    tf.seedEnd = tf.seedStart+config.SEED_RANGE();
    tf.popSize = config.POP_SIZE();
    tf.gens = config.GENS();
    tf.filePath = config.FILE_PATH();
    tf.fileName = config.FILE_NAME();
    tf.fileFile = config.FILE_FILE();
    tf.dc = dataCenter;
    threadList.push_back(std::thread([tf]() 
    {
      for(int i = tf.seedStart; i < tf.seedEnd; i++){
        evolve(i,tf.popSize,tf.gens,tf.filePath,tf.fileName,tf.fileFile,tf.dc);
        std::cout << "yay! ";
      }
    }));
  }
  std::for_each(threadList.begin(), threadList.end(), [](std::thread &t) 
  {
    t.join();
  });

  dataCenter.combineFiles(config.SEED(),config.SEED()+config.THREAD_CT()*config.SEED_RANGE(),
                          config.FILE_PATH(),config.FILE_NAME());

  std::cout << "k-means implemented!" << std::endl;
}