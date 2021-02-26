#pragma once

#include <iostream>
#include <string>

#include "emp/base/vector.hpp"
#include "emp/math/Random.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/data/DataFile.hpp"

#include "selection.h"
#include "Organism.h"
#include "DataCenter.h"

void evolve(int seed, size_t popSizeSet, size_t genSet, 
            std::string fPath, std::string fName, std::string fileFile, DataCenter dataCenter) {
  const size_t population_size = popSizeSet;
  const size_t gens = genSet;
  size_t curr_gen = 0;

  // make random engine
  emp::Random rand(seed);

  emp::vector<Organism> population;
  for(size_t i = 0; i < population_size; i++){
    Organism org = Organism(&rand, dataCenter, 
            emp::RandomDoubleVector(rand,dataCenter.getCounter(),-1,1), 
            emp::RandomDoubleVector(rand,dataCenter.getCounter(),0.002,0.01));
    population.push_back(org);
  }

  auto datafile = emp::MakeContainerDataFile(
    std::function<emp::vector<Organism>()>{
      [&population](){ return population; }
    },
    (fPath+"Org_Vals"+std::to_string(seed)+fName)
  );

  datafile.AddVar(
    curr_gen,
    "generation",
    "Current Generation"
  );

  for(size_t i = 0; i < dataCenter.getCounter(); i++){
    std::string wt = "weight"+std::to_string(i);
    std::string dv = "deviation"+std::to_string(i);
    datafile.AddContainerFun(
      std::function<double(Organism)>{[i](Organism x){
        return x.getWeights().at(i);
      }}, wt, "Genome's content"
    );
    datafile.AddContainerFun(
      std::function<double(Organism)>{[i](Organism x){
        return x.getDeviations().at(i);
      }}, dv, "Genome's content"
    );
  }

  datafile.AddContainerFun(
    std::function<double(Organism)>{[](Organism x){
      return x.calcFitness();
    }},
    "fitness",
    "Genome's Fitness"
  );

  datafile.PrintHeaderKeys();
  datafile.Update();

  while (++curr_gen < gens) {
    emp::vector<Organism> next_population;

    // select individuals for next generation
    for (size_t i = 0; i < population_size; ++i) {
      Organism winner = doTournament(
        population,
        rand,
        sqrt(population_size)
      );
      next_population.push_back(winner);
    }

    // do mutation
    for (Organism& org : next_population) {
      org.mutate();
    }

    population = next_population;
    if(curr_gen%100 == (genSet-1)%100){datafile.Update();}
  }
}
