#pragma once

#include <math.h>
#include "emp/math/Random.hpp"

class Organism{
    private:
        double predictAdd;
        double predictMult;
        double chanceAdd;
        double chanceMult;
        double mutateAmt = 0.002;
        emp::Ptr<emp::Random> rand;

    public:
    Organism(emp::Ptr<emp::Random> random, double pA = 0.5, double pM = 1.0, double cA = 0.5, double cM = 0.5) :
        rand(random), predictAdd(pA), predictMult(pM), chanceAdd(cA), chanceMult(cM) {;}
    Organism(const Organism &) = default;
    Organism(Organism &&) = default;

    Organism & operator=(const Organism &) = default;
    Organism & operator=(Organism &&) = default;
    bool operator==(const Organism &other) const { return (this == &other);}
    bool operator!=(const Organism &other) const {return !(*this == other);}

    double getPA(){return predictAdd;}
    double getPM(){return predictMult;}
    double getCA(){return chanceAdd;}
    double getCM(){return chanceMult;}

    void setAdd(double val) {predictAdd = val;}
    void setMult(double val) {predictMult = val;}
    void setAddC(double val) {chanceAdd = val;}
    void setMultC(double val) {chanceMult = val;}

    void mutate(){
        predictAdd += rand->GetRandNormal(0.0,mutateAmt);
        predictMult += rand->GetRandNormal(0.0,mutateAmt);
        chanceAdd += rand->GetRandNormal(0.0,mutateAmt);
        chanceMult += rand->GetRandNormal(0.0,mutateAmt);
        if(chanceAdd > 1) chanceAdd = 1.0;
        if(chanceAdd < 0) chanceAdd = 0.0;
        if(chanceMult > 1) chanceMult = 1.0;
        if(chanceMult < 0) chanceMult = 0.0;
    }

    double calcFitness(double start = 1, double end = 4){
        double value = start;
        if(rand->GetDouble() < chanceMult){
            value*=predictMult;
        }
        if(rand->GetDouble() < chanceAdd){
            value+=predictAdd;
        }
        return 1-pow((value-end),2);
    } 
};