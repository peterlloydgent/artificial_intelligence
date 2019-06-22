//
//  Perceptron.hpp
//  neuralnetworks
//
//  Created by Peter Lloyd Gent on 12/11/2018.
//  Copyright © 2018 Peter Lloyd Gent. All rights reserved.
//

#ifndef Perceptron_hpp
#define Perceptron_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <cmath>


using namespace std;

class Perceptron
{
    public:
        Perceptron();
        void setInputs(vector<double>& weightParam,vector<double>& pwu, vector<Perceptron*>& inputParam, double biasParam);
        void feedForward();
        void adjustWeights(double currentError);
        double getIncommingWeight(Perceptron* perceptron);
        double threshold(double input);
        inline int getInputSize(){return inputs.size();}
        inline double getError(){return error;}
        inline void setState(double newState){state = newState;}
        inline double getState(){return state;}
        inline int getID(){return perceptronNo;}
        inline double getBias(){return bias;}
        inline void setBias(double inputbias){bias = inputbias;}
        inline vector<double> getWeights(){return weights;}
        inline void setWeights(vector<double> inputweights){weights = inputweights;}
        inline vector<Perceptron*> getPerceptrons(){return inputs;}
    private:
        int perceptronNo;
        vector<double> weights;
        double bias;
        vector<Perceptron*> inputs;
        constexpr const double static LEARNINGRATE = 0.06; // started with 0.1 and changed each time until optimal value was found
        constexpr const double static alpha = 0.25; // momentum parameter. GDM is important to make sure that it moves more queickly to minimum - builds in overall pattern
        double error;
        double state;
        static int perceptronCount;
        vector<double> previous_weight_update;
        double previous_bias_update;
};

#endif /* Perceptron_hpp */
