#include <Aria.h>
#include <iostream>
#include <conio.h>
#include "NeuralNetwork.hpp"
using namespace std;

int main(int argc, char** argv)
{
	Aria::init();
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();
	ArRobot aRobot;
	ArRobotConnector robotConnector(&parser, &aRobot);
	// Try connecting to the robot.

	NeuralNetwork n;
	//string filenameInput = "rangesTranspose.csv";
	//string filenameOutput = "motorSpeedTranspose.csv";
	try
	{
		/*fstream fileInput = NeuralNetwork::getFile(filenameInput);
		vector<pair<double, double>> transposeRange = NeuralNetwork::getCSVValues(fileInput);
		fstream fileOutput = NeuralNetwork::getFile(filenameOutput);
		vector<pair<double, double>> transposeMotorSpeed = NeuralNetwork::getCSVValues(fileOutput);
		for(int i =0 ;i<5; ++i)
			n.learn(transposeRange, transposeMotorSpeed);
			//C:\Users\wendy\Desktop\neuralnetworks
		*/
		string filenameInput1 = "../../nntrainingdata.csv";
		fstream fileInput1 = NeuralNetwork::getFile(filenameInput1);
		NeuralNetwork::removeDuplicates(fileInput1); 
		fileInput1.close();
		string filenameInputFinal = "../../nntrainingdatafinal.csv";
		fstream fileInput2 = NeuralNetwork::getFile(filenameInputFinal);
		pair<vector<pair<double, double>>, vector<pair<double, double>>> nntraining = NeuralNetwork::extractFromStandardCSV(fileInput2);
		fileInput2.close();
		n.preprocessCSV(nntraining);
		NeuralNetwork::randomize(nntraining);
		ofstream mse_out;
		mse_out.open("../../mse_out_4neurones.csv"); //!!
		for (int i = 0; i < 405; ++i) {
			cout << i << endl;
			mse_out << endl << i << ',';
			double mse;
			n.learn(nntraining.first, nntraining.second);
			mse = n.meanSquaredError(nntraining.first, nntraining.second);
			cout << mse << endl;
			mse_out << mse << ' ' <<  endl;
		}
		mse_out.close();
		n.saveWeightsToFile("../../nnweights4neurones");//!!
		//n.readWeightsFromFile("C:/Users/charl/Code/nnweights.txt");
		if (!robotConnector.connectRobot(&aRobot))
		{
			// Error!
			ArLog::log(ArLog::Terse, "Error, could not connect to robot.\n");
			robotConnector.logOptions();
			Aria::exit(1);
		}
		else
		{
			aRobot.runAsync(false);
			aRobot.lock();
			aRobot.enableMotors();
			aRobot.unlock();

			while (true)
			{
				ArSensorReading *sonarSensor[8];
				int sonarRange[8];
				for (int i = 0;i < 2;++i)
				{
					sonarSensor[i] = aRobot.getSonarReading(i);
					sonarRange[i] = sonarSensor[i]->getRange();
				}
				pair<double,double> input = pair<double,double>(sonarRange[0],sonarRange[1]);
				pair<double, double> output = n.decentredOutput(input);
				cout << "Output 1 " << output.first << endl;
				cout << "Output 2 " << output.second << endl;
				aRobot.setVel2(output.first,output.second);
				ArUtil::sleep(100);
			}
		}

	}
	catch (exception& e)
	{
		cout << "captured exception " << endl;
		cout << e.what() << endl;
	}
}
