//
//  NeuralNetwork.hpp
//  neuralnetworks
//
//  Created by Peter Lloyd Gent on 12/11/2018.
//  Copyright © 2018 Peter Lloyd Gent. All rights reserved.
//

#ifndef NeuralNetwork_hpp
#define NeuralNetwork_hpp


#include "Perceptron.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <time.h>
#include <map>
#include <algorithm>

class NeuralNetwork
{
    private:
        std::vector<Perceptron> inputPerceptrons;
        std::vector<Perceptron> hiddenPerceptrons;
        std::vector<Perceptron> outputPerceptrons;
        const int INPUTNEURONCOUNT = 2;
        const int HIDDENNEURONCOUNT = 4;//!!!
        const int OUTPUTNEURONCOUNT = 2;
        double input1Max, input2Max, output1Max, output2Max;
    public:
        NeuralNetwork();
        void getNeuralNetworkIdentifiers();
        void generatedOutput(pair<double,double>& input);
        void backProp(pair<double,double>& expectedOutput);
        void learn(vector<pair<double,double>>& inputs,vector<pair<double, double>>& outputs);
		pair<double, double> processInput(pair<double, double>& input);
		void checkOutput();
        void printWeights();
        void printError(pair<double,double> output, int i);
        void instantializeInputLayer();
        void instantializeHiddenLayer();
        void instantializeOutputLayer();
        static fstream getFile(string& name);
        static double getError(double input,double state);
        static vector<pair<double,double>> getCSVValues(fstream& file);
        static double fRand(double fMin, double fMax);
        static void removeDuplicates(fstream& inputcsv);
        static pair<vector<pair<double,double>>,vector<pair<double,double>>> extractFromStandardCSV(fstream& file);
        static double max_first(vector<pair<double,double>>& inputvector);
        static double max_second(vector<pair<double,double>>& inputvector);
        void preprocessCSV(pair<vector<pair<double,double>>,vector<pair<double,double>>>& input);
        static void randomize(pair<vector<pair<double,double>>,vector<pair<double,double>>>& input);
        double meanSquaredError(vector<pair<double,double>>& input,vector<pair<double,double>>& modelOutput);
        void readWeightsFromFile(string filename);
        void saveWeightsToFile(string filename);
        pair<double,double> decentredOutput(pair<double,double>& input); // wraper around process input. timese d by a 1/2 to restrict range between .25 and .75 of output values to not end up in a region where there is a poor gradient.
};
#endif /* NeuralNetwork_hpp */
