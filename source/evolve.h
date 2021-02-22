#pragma once

#include <iostream>

#include "emp/base/vector.hpp"
#include "emp/math/Random.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/data/DataFile.hpp"

#include "selection.h"
#include "Organism.h"

void evolve() {
  const size_t population_size = 50;
  const size_t gens = 6000;
  size_t curr_gen = 0;

  // make random engine
  emp::Random rand(1);

  // vector to store our population,
  // fill it with randomized organisms
  emp::vector<Organism> population;
  for(size_t i = 0; i < population_size; i++){
    Organism org = Organism(&rand);
    population.push_back(org);
  }
  // emp::vector<double> population;
  // population = emp::RandomDoubleVector(
  //   rand,
  //   population_size,
  //   0.0,
  //   1.0
  // );

  auto datafile = emp::MakeContainerDataFile(
    std::function<emp::vector<Organism>()>{
      [&population](){ return population; }
    },
    "evo-algo.csv"
  );

  datafile.AddVar(
    curr_gen,
    "generation",
    "Current Generation"
  );

  datafile.AddContainerFun(
    std::function<double(Organism)>{[](Organism x){
      return x.getPA();
    }},
    "PA",
    "Genome's content"
  );

  datafile.AddContainerFun(
    std::function<double(Organism)>{[](Organism x){
      return x.getCA();
    }},
    "CA",
    "Genome's content"
  );

  datafile.AddContainerFun(
    std::function<double(Organism)>{[](Organism x){
      return x.getPM();
    }},
    "PM",
    "Genome's content"
  );

  datafile.AddContainerFun(
    std::function<double(Organism)>{[](Organism x){
      return x.getCM();
    }},
    "CM",
    "Genome's content"
  );

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
        3
      );
      next_population.push_back(winner);
    }

    // do mutation
    for (Organism& org : next_population) {
      org.mutate();
    }

    population = next_population;

    datafile.Update();

  }
}
