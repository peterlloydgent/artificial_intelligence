#include "Perceptron.hpp"

int Perceptron::perceptronCount = 0;

Perceptron::Perceptron()
{
	perceptronNo = perceptronCount++;
	error = 0.0;
	state = 0.0;
	bias = 0.0;
	weights = vector<double>();
	previous_weight_update = vector<double>();
	previous_bias_update= 0.0;
}

void Perceptron::feedForward()
{
	double sum=0.0;
	for(int i=0;i<static_cast<int>(inputs.size());++i)
	{
		double weightTimes = weights[i];
		sum += inputs[i]->getState() *weightTimes;
	}
	state = threshold(sum + bias);
}
// needs lambda.
double Perceptron::threshold(double input)
{
	return 1.0 / (1.0 + exp(-input));
}

void Perceptron::adjustWeights(double currentError)
{
	for(int i=0;i<static_cast<int>(inputs.size());++i)
	{
		double deltaWeight = LEARNINGRATE * currentError * inputs[i]->getState() + alpha*previous_weight_update[i];
		double newWeight = weights[i];
		previous_weight_update[i] = deltaWeight;
		newWeight+=deltaWeight;
		weights[i] = newWeight;
		bias += LEARNINGRATE * currentError + alpha* previous_bias_update;
		previous_bias_update = LEARNINGRATE * currentError + alpha* previous_bias_update;
	}
}

double Perceptron::getIncommingWeight(Perceptron* incomming)
{
	for(int i=0;i<static_cast<int>(inputs.size());++i)
	{
		if(incomming==inputs[i])
		{
			return weights[i];
		}
	}
	return 0;
}

void Perceptron::setInputs(vector<double> &weightParam,vector<double> &pwu,vector<Perceptron*>& inputParam, double biasParam)
{
	inputs = inputParam;
	weights = weightParam;
	previous_weight_update = pwu;
	bias = biasParam;
}
