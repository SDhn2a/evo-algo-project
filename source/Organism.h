#pragma once

#include <math.h>
#include "emp/base/vector.hpp"
#include "emp/math/Random.hpp"
#include "DataCenter.h"
#define MIN 0.0001

class Organism{
    private:
        emp::Ptr<emp::Random> rand;
        emp::vector<double> weights;
        emp::vector<double> deviations;
        double mutateAmt = 0.5;
        DataCenter hub;

    public:
    Organism(emp::Ptr<emp::Random> random, DataCenter dataCenter, emp::vector<double> w, emp::vector<double> d) :
        rand(random), weights(w), deviations(d), hub(dataCenter) {;}
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
        return hub.evaluate(weights,deviations,rand);
    }
};