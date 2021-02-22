#pragma once

#include <algorithm>

#include "emp/base/vector.hpp"
#include "emp/math/Random.hpp"
#include "emp/math/random_utils.hpp"

#include "Organism.h"

Organism doTournament(
  const emp::vector<Organism>& population,
  emp::Random& rand,
  const size_t tournament_size = 7
) {

	emp::vector<size_t> choices = emp::Choose(
		rand,
		population.size(),
		tournament_size
	);

  emp::vector<Organism> selected;
	for (const size_t& x: choices) {
		selected.push_back(population[x]);
	}

	Organism winner = *std::max_element(
		std::begin(selected),
		std::end(selected),
		[](Organism a, Organism b) {
			return (a.calcFitness() < b.calcFitness());
		}
	);

	return winner;
}
