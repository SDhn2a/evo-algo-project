#pragma once

#include <math.h>
#include "emp/base/vector.hpp"
#include "emp/math/Random.hpp"
#include "DataCenter.h"
#define MIN 0.0001

class Organism{
    private:
        emp::Ptr<emp::Random> rand;
        emp::Ptr<DataCenter> hub;
        size_t seedPos;
        emp::vector<double> weights;
        emp::vector<double> deviations;
        double mutateAmt = 0.5;
        double fitness;

    public:
    Organism(emp::Ptr<emp::Random> random, emp::Ptr<DataCenter> dataCenter, size_t sp, emp::vector<double> w, emp::vector<double> d) :
        rand(random), hub(dataCenter), seedPos(sp), weights(w), deviations(d) {;}
    Organism(const Organism &) = default;
    Organism(Organism &&) = default;

    Organism & operator=(const Organism &) = default;
    Organism & operator=(Organism &&) = default;
    bool operator==(const Organism &other) const { return (this == &other);}
    bool operator!=(const Organism &other) const {return !(*this == other);}

    emp::vector<double> getWeights(){return weights;}
    emp::vector<double> getDeviations(){return deviations;}

    void setWeights(emp::vector<double> val){weights = val;}
    void setDeviations(emp::vector<double> val){deviations = val;}

    void mutate(){
        for(size_t i = 0; i < weights.size(); i++){
            weights.at(i) += rand->GetRandNormal(0.0,deviations.at(i)/10);
            deviations.at(i) += rand->GetRandNormal(0.0,mutateAmt);
            if(deviations.at(i) < MIN) deviations.at(i) = MIN;
        }
        mutateAmt = std::max(0.98*mutateAmt, 0.0002);
    }

    double calcFitness(){
        fitness = hub->evaluate(weights,deviations,rand,seedPos);
        return fitness;
    }

    double getFitness(){
        return fitness;
    }
};