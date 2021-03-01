#pragma once

#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <algorithm>

#include "emp/base/vector.hpp"
#include "emp/data/DataFile.hpp"
#include "emp/io/File.hpp"
#include "emp/math/Random.hpp"
#include "asa058.hpp"
#include "asa058.cpp"

struct dataPoint{
    emp::vector<int> nodeMap;
    emp::vector<double> values;
};

struct IODataPoint{
    emp::vector<int> input;
    emp::vector<int> output;
    emp::vector<int> open;
    emp::vector<double> values;
};

class DataCenter{
    private:
        emp::vector<std::string> titles;
        emp::vector<emp::vector<dataPoint>> dataPile;
        emp::vector<emp::vector<int>> connectivity;
        emp::vector<dataPoint> dataSample;
        emp::vector<IODataPoint> IODataSample;
        size_t counter;
    
    public:

    size_t getCounter(){return counter;}

    emp::File kMeans(std::string fileName, std::string fPath){
        std::string kMeansName = (fPath+"k_"+fileName);
        emp::File incoming(fPath+fileName);
        std::cout << fileName << "\n";
        emp::File outgoing;
        //parse
        incoming.ExtractCol();
        std::vector<std::string> header = incoming.ExtractRow();
        incoming.RemoveEmpty();
        int observations = incoming.GetNumLines();
        int variables = header.size();
        std::vector<double> valuesToAvg[variables-1];
        for(int i = 0; i < variables-1; i++){
            std::vector<std::string> stringToValue = incoming.ExtractCol();
            std::vector<double> valueToAdd;
            for(size_t j = 0; j < DATASIZE; j++){
                valueToAdd.push_back(stod(stringToValue.at((9973*j+1) % observations)));
            }
            valuesToAvg[i] = valueToAdd;
        }
        std::vector<std::string> stringValues = incoming.ExtractCol();
        double values[DATASIZE] = {0};
        for(size_t i = 0; i < DATASIZE; i++){
            values[i] = stod(stringValues.at((9973*i+1) % observations));
        }
        double clusterCenters[CLUSTERS] = {0};
        for(int i = 0; i < CLUSTERS; i++){
            clusterCenters[i] = values[i];
        }
        double deviations[CLUSTERS] = {0};
        int assignments[DATASIZE] = {0};
        double workspace[DATASIZE] = {0};
        int clusterSize[CLUSTERS] = {0};
        clustr(values,clusterCenters,deviations,assignments,workspace,clusterSize);
        double mean = 0;
        outgoing.Append("Clusters:\n");
        for(size_t i = 0; i < CLUSTERS; i++){
            outgoing.Append(to_string(clusterCenters[i])+","+to_string(clusterSize[i])+","+to_string(deviations[i]));
            mean += (clusterSize[i]/pow(deviations[i],2))/CLUSTERS;
        }
        std::cout << "\n\n";
        for(size_t i = 0; i < DATASIZE; i++){
            int clusterAssignment = assignments[i]-1;
            // double meanArray[variables-1];
            // double devArray[variables-1];
            std::cout << clusterAssignment << "::";
            if(clusterSize[clusterAssignment]/pow(deviations[clusterAssignment],2) >= mean){
                std::cout << i << "  ";
                // for(int j = 0; j < variables-1; j++){
                //     meanArray[j] += valuesToAvg[j].at(i)/clusterSize[clusterAssignment];
                // }
            }
        }
        std::cout << "\n\n";
        outgoing.Append("\nOptimal Values:\n");
        for(int i = 0; i < CLUSTERS; i++){
            if(clusterSize[i]/pow(deviations[i],2) >= mean){
                std::string stringToAdd;
                for(int j = 0; j < variables-1; j++){
                    double varMean = 0;
                    double varDev = 0;
                    for(size_t k = 0; k < DATASIZE; k++){
                        if(assignments[k]-1 == i){
                            varMean += valuesToAvg[j].at(k)/clusterSize[i];
                        }
                    }
                    for(size_t k = 0; k < DATASIZE; k++){
                        if(assignments[k]-1 == i){
                            varDev += pow(varMean-valuesToAvg[j].at(k),2)/(clusterSize[i]-1);
                        }
                    }
                    varDev = pow(varDev,0.5);
                    stringToAdd += to_string(varMean) + "  " + to_string(varDev) + "  ";
                }
                outgoing.Append(stringToAdd);
            }
        }
        outgoing.Write(kMeansName);
        return outgoing;
    }

    emp::File combineFiles(int startInt, int endInt, std::string fPath, std::string fName){
        std::string combinedFileName = (fPath+"Range_"+std::to_string(startInt)+"_"+std::to_string(endInt)+fName);
        emp::File combined;
        for(int i = startInt; i < endInt; i++){
            std::string fileName = (fPath+"Org_Vals"+std::to_string(i)+fName);
            emp::File tempFile = emp::File(fileName);
            if(i > startInt){tempFile.ExtractRow();}
            combined.Append(tempFile);
        }
        combined.RemoveEmpty();
        combined.Write(combinedFileName);
        std::string fileToKMeans = "Range_"+std::to_string(startInt)+"_"+std::to_string(endInt)+fName;
        kMeans(fileToKMeans,fPath);
        return combined;
    }

    double evaluate(emp::vector<double> weights, emp::vector<double> deviations, emp::Ptr<emp::Random> random){
        if(weights.size()<counter || weights.size()>counter || 
           deviations.size()<counter || deviations.size()>counter){return -1;}
        emp::vector<emp::vector<double>> weightMatrix;
        emp::vector<emp::vector<double>> devMatrix;
        int matrixIter = 0;
        for(size_t i = 0; i < connectivity.size(); i++){
            emp::vector<double> weightVector(connectivity.size());
            emp::vector<double> devVector(connectivity.size());
            weightMatrix.push_back(weightVector);
            devMatrix.push_back(devVector);
        }
        //Defining the weight and deviation matrices
        for(size_t i = 0; i < connectivity.size(); i++){
            for(size_t j = 0; j < connectivity.size(); j++){
                if(connectivity.at(i).at(j) == 1){
                    weightMatrix.at(i).at(j) = weights.at(matrixIter);
                    devMatrix.at(i).at(j) = deviations.at(matrixIter);
                    matrixIter++;
                }
            }
        }
        double distance = 0;
        for(size_t i = 0; i < IODataSample.size(); i++){
            //actually evaluating
            IODataPoint dt = IODataSample.at(i);
            emp::vector<double> nodeValues(connectivity.size()); //one value for every node
            emp::vector<bool> nodeValuesInput(connectivity.size()); //true if nodeValues has been filled in the corresponding spot
            //put input into input nodes
            for(size_t j = 0; j < dt.input.size(); j++){
                nodeValues.at(dt.input.at(j)) = dt.values.at(dt.input.at(j));
                nodeValuesInput.at(dt.input.at(j)) = true;
            }
            //until all outputs are filled
            emp::vector<int> tempout = dt.output;
            while(tempout.size() > 0){
                //for each node with all inputs filled in
                for(size_t j = 0; j < dt.open.size(); j++){
                    bool ready = true;
                    for(size_t k = 0; k < connectivity.size(); k++){
                        if(connectivity.at(k).at(dt.open.at(j)) == 1 && !nodeValuesInput.at(k)){
                            ready = false;
                        }
                    }
                    //give random normal output
                    if(ready){
                        double sum = 0;
                        for(size_t k = 0; k < connectivity.size(); k++){
                            if(connectivity.at(k).at(dt.open.at(j)) == 1){
                                sum += (nodeValues.at(k)
                                        +random->GetRandNormal(0.0,devMatrix.at(k).at(dt.open.at(j))))
                                        *weightMatrix.at(k).at(dt.open.at(j));
                            }
                        }
                        nodeValues.at(dt.open.at(j)) = sum;
                        nodeValuesInput.at(dt.open.at(j)) = true;
                    }
                }
                for(size_t q = tempout.size(); q > 0; q--){
                    size_t j = q-1;
                    bool ready = true;
                    for(size_t k = 0; k < connectivity.size(); k++){
                        if(connectivity.at(k).at(tempout.at(j)) == 1 && !nodeValuesInput.at(k)){
                            ready = false;
                        }
                    }
                    //give random normal output
                    if(ready){
                        double sum = 0;
                        for(size_t k = 0; k < connectivity.size(); k++){
                            if(connectivity.at(k).at(tempout.at(j)) == 1){
                                sum += (nodeValues.at(k)
                                        +random->GetRandNormal(0.0,devMatrix.at(k).at(tempout.at(j))))
                                        *weightMatrix.at(k).at(tempout.at(j));
                            }
                        }
                        nodeValues.at(tempout.at(j)) = sum;
                        nodeValuesInput.at(tempout.at(j)) = true;
                        tempout.erase(tempout.begin()+j);
                    }
                }
            }
            //take distance from output nodes
            for(size_t j = 0; j < dt.output.size(); j++){
                double sum = 0;
                for(size_t k = 0; k < connectivity.size(); k++){
                    if(connectivity.at(k).at(dt.output.at(j)) == 1){
                        sum += devMatrix.at(k).at(dt.output.at(j))*abs(weightMatrix.at(k).at(dt.output.at(j)));
                    }
                }
                if(sum > 0){
                    distance += pow(dt.values.at(dt.output.at(j))-nodeValues.at(dt.output.at(j)),2)/sqrt(sum);
                }
            }
        }
        return 1-distance/IODataSample.size();
    }

    void sampleData(){
        for(size_t i = 0; i < dataPile.size(); i++){
            //Create a random permutation
            emp::vector<unsigned int> indices(dataPile.at(i).size());
            std::iota(indices.begin(), indices.end(), 0);
            std::random_shuffle(indices.begin(), indices.end());
            // std::cout << "sqrt = " << sqrt(dataPile.at(i).size()) << "\n";
            for(double j = 0; j < sqrt(dataPile.at(i).size()); j++){
                //Add data to dataSample
                // std::cout << "index = " << indices.at(j) << "\n";
                // std::cout << "values = " << dataPile.at(i).at(indices.at(j)).values.at(0) << " "
                //                          << dataPile.at(i).at(indices.at(j)).values.at(1) << " "
                //                          << dataPile.at(i).at(indices.at(j)).values.at(2) << "\n";
                dataSample.push_back(dataPile.at(i).at(indices.at(j)));
            }
        }
    }

    void dataToIOData(int pick){
        //For each data point
        for(size_t i = 0; i < dataSample.size(); i++){
            //Determine used and unused columns
            emp::vector<int> columns;
            emp::vector<int> openCol;
            for(size_t j = 0; j < dataSample.at(i).nodeMap.size(); j++){
                columns.push_back(dataSample.at(i).nodeMap.at(j));
            }
            for(size_t j = 0; j < connectivity.size(); j++){
                bool used = false;
                int jShift = j;
                for(size_t k = 0; k < columns.size(); k++){
                    if(jShift == columns.at(k)){
                        used = true;
                    }
                }
                if(!used){openCol.push_back(j);}
            }
            //Determine fixed input/output columns
            emp::vector<int> outputColumns;
            emp::vector<int> inputColumns;
            emp::vector<int> resolvedColumns;
            for(size_t j = 0; j < columns.size(); j++){
                bool noOutput = true;
                bool noInput = true;
                //Check for terminal output, empty input
                for(size_t k = 0; k < columns.size(); k++){
                    if(connectivity.at(columns.at(j)).at(columns.at(k)) == 1){
                        noOutput = false;
                    }
                    if(connectivity.at(columns.at(k)).at(columns.at(j)) == 1){
                        noInput = false;
                    }
                }
                if(noOutput) {outputColumns.push_back(columns.at(j));}
                if(noInput) {inputColumns.push_back(columns.at(j));}
                if(noInput || noOutput) {resolvedColumns.push_back(columns.at(j));}
            }
            //While there are columns remaining
            while(resolvedColumns.size() < columns.size()){
                emp::vector<int> unassignedColumns;
                for(size_t j = 0; j < columns.size(); j++){
                    bool resolved = false;
                    for(size_t k = 0; k < resolvedColumns.size(); k++){
                        if(columns.at(j) == resolvedColumns.at(k)){
                            resolved = true;
                        }
                    }
                    //Combine the unresolved columns
                    if (!resolved){
                        unassignedColumns.push_back(columns.at(j));
                    }
                }
                //If a column has only resolved columns as output, set to output + resolved column
                //If a column has only resolved columns as input, set to output + resolved column
                int additions = 0;
                for(size_t j = 0; j < unassignedColumns.size(); j++){
                    bool inputResolved = true;
                    bool outputResolved = true;
                    for(size_t k = 0; k < unassignedColumns.size(); k++){
                        if(connectivity.at(unassignedColumns.at(j)).at(unassignedColumns.at(k)) == 1) {outputResolved = false;}
                        if(connectivity.at(unassignedColumns.at(k)).at(unassignedColumns.at(j)) == 1) {inputResolved = false;}
                    }
                    if(inputResolved || outputResolved) {
                        additions++;
                        outputColumns.push_back(unassignedColumns.at(j));
                        resolvedColumns.push_back(unassignedColumns.at(j));
                    }
                }
                //Once these are exhausted, assign a column as input + resolved column, then resume attempts
                if(additions == 0){
                    inputColumns.push_back(unassignedColumns.at(pick % unassignedColumns.size()));
                    resolvedColumns.push_back(unassignedColumns.at(pick % unassignedColumns.size()));
                }
            }
            IODataPoint newDataPoint;
            newDataPoint.input = inputColumns;
            newDataPoint.output = outputColumns;
            newDataPoint.open = openCol;
            emp::vector<double> fullValues(connectivity.size());
            for(size_t j = 0; j < dataSample.at(i).values.size(); j++){
                fullValues.at(dataSample.at(i).nodeMap.at(j)) = dataSample.at(i).values.at(j);
            }
            newDataPoint.values = fullValues;
            IODataSample.push_back(newDataPoint);
        }
    }

    void standardizeDataByFile(){
        //for each file
        for(size_t i = 0; i < dataPile.size(); i++){
            emp::vector<double> means;
            emp::vector<double> deviations;
            for(size_t j = 0; j < dataPile.at(i).at(0).values.size(); j++){
                means.push_back(0);
                deviations.push_back(0);
            }
            //for each column
            for(size_t j = 0; j < dataPile.at(i).at(0).values.size(); j++){
                //find mean
                double sum = 0;
                for(size_t k = 0; k < dataPile.at(i).size(); k++){
                    sum += dataPile.at(i).at(k).values.at(j);
                }
                sum /= dataPile.at(i).size();
                means.at(j) = sum;
                //for each element
                double deviation = 0;
                for(size_t k = 0; k < dataPile.at(i).size(); k++){
                    //find distance from mean, squared
                    deviation += pow(sum - (dataPile.at(i).at(k).values.at(j)),2);
                }
                deviation /= dataPile.at(i).size()-1; //Minus one for Bessel's correction
                deviation = pow(deviation, 0.5);
                deviations.at(j) = deviation;
            }
            //Revise dataPoints
            for(size_t k = 0; k < dataPile.at(i).size(); k++){
                for(size_t j = 0; j < dataPile.at(i).at(0).values.size(); j++){
                    if(deviations.at(j) == 0){
                        dataPile.at(i).at(k).values.at(j) = 0;
                    }
                    else{
                        dataPile.at(i).at(k).values.at(j) -= means.at(j);
                        dataPile.at(i).at(k).values.at(j) /= deviations.at(j);
                    }
                }
            }
        }
    }

    int makeConnectivity(std::string fileName){
        emp::File connect = emp::File(fileName);
        emp::vector<int> blank;
        counter = 0;
        for(size_t i = 0; i < titles.size(); i++){
            blank.push_back(0);
        }
        for(size_t i = 0; i < titles.size(); i++){
            connectivity.push_back(blank);
        }
        for(size_t i = connect.GetNumLines(); i > 0; i--){
            emp::vector<std::string> link = connect.ExtractRow();
            for(size_t j = 0; j < titles.size(); j++){
                if(titles.at(j) == link.at(0)){
                    for(size_t k = 0; k < titles.size(); k++){
                        if(titles.at(k) == link.at(1)){
                            if(connectivity.at(j).at(k) == 0){
                                counter++;
                            }
                            connectivity.at(j).at(k) = 1;
                            k = titles.size();
                        }
                    }
                    j = titles.size();
                }
            }
        }
        // for(size_t i = 0; i < connectivity.size(); i++){
        //     for(size_t j = 0; j < connectivity.size(); j++){
        //         std::cout << connectivity.at(i).at(j) << " ";
        //     }
        //     std::cout << "\n";
        // }
        return counter;
    }

    void importData(std::string fileFile){
        //Import files
        emp::File meta = emp::File(fileFile);
        emp::vector<std::string> fileNames = meta.ExtractCol();
        emp::vector<emp::File> fileVector;
        emp::vector<emp::vector<std::string>> columnHeaders;
        for(size_t i = fileNames.size(); i > 0; i--){
            emp::File tempFile = emp::File(fileNames.back());
            fileNames.pop_back();
            fileVector.push_back(tempFile);
        }
        //Read off first row of each, assemble titles
        for(size_t i = 0; i < fileVector.size(); i++){
            emp::vector<std::string> header = fileVector.at(i).ExtractRow();
            columnHeaders.push_back(header);
            for(size_t i = 0; i < header.size(); i++){
                titles.push_back(header.at(i));
            }
        }
        //Collapse identical titles
        std::sort(titles.begin(), titles.end());
        titles.erase(std::unique(titles.begin(), titles.end()), titles.end());
        //For each file
        for(size_t i = 0; i < fileVector.size(); i++){
            //For each row, add a dataPoint to the dataPile
            //The nodeMap points to the adjusted column (so that all files have consistent column labeling)
            emp::vector<int> makeNodeMap;
            for(size_t j = 0; j < columnHeaders.at(i).size(); j++){
                for(size_t k = 0; k < titles.size(); k++){
                    if(columnHeaders.at(i).at(j) == titles.at(k)){
                        makeNodeMap.push_back(k);
                    }
                }
            }
            //The values provides the list of actual values
            emp::vector<dataPoint> smallDataPile;
            for(size_t j = fileVector.at(i).GetNumLines(); j >0; j--){
                dataPoint newDataPoint;
                //Need to convert from string to double
                emp::vector<std::string> stringValues = fileVector.at(i).ExtractRow();
                newDataPoint.values.reserve(stringValues.size());
                transform(stringValues.begin(), stringValues.end(), back_inserter(newDataPoint.values),
                        [](std::string const& val) {return std::stod(val);});
                //Insert the column values
                newDataPoint.nodeMap = makeNodeMap;
                smallDataPile.push_back(newDataPoint);
            }
            dataPile.push_back(smallDataPile);
        }
    }
};