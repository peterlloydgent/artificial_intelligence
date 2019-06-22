#include <Aria.h>
#include "ariaUtil.h"
#include <fstream>
#include <cassert>

using namespace std;


const int DESIREDDISTANCE = 500;
const double DISTANCEBETWEENWHEELS = 260;
const double BASEVELOCITY = 200;
const double COSONSONARSENSOR6ANGLE = 0.766044443;
const double KP = 0.6;
const double KI = 0.001;
const double KD = 0.001;

double errorprev = 0;
double currente = 0;
double errori = 0;
double errord = 0;
ofstream errorFile;

// right wall following behaviour.
double* pid(int first,int second)
{
	double* output = new double[2];
	double smallestSensorReading = first < second ? first : second;
	double errorMinDistance = DESIREDDISTANCE - smallestSensorReading; // take mean distance for both readings.
	errorFile << "first " << first << " " << " second " << second;
	errorFile << "error min distance: " << errorMinDistance << "; ";
	errorFile << "error i prior: " << errori << "; ";
	errori = errori + errorMinDistance;
	errorFile << "error i post: " << errori << "; ";
	errord = errorMinDistance - errorprev;
	errorprev = errorMinDistance;

	double outputK = KP * errorMinDistance + KI*errori + errord*KD;
	errorFile << "outputK " << outputK << "; ";
	double angularVelocity = outputK / 130;
	double leftVel = BASEVELOCITY - ((DISTANCEBETWEENWHEELS * angularVelocity) / 2);
	double rightVel = BASEVELOCITY + ((DISTANCEBETWEENWHEELS * angularVelocity) / 2);
	errorFile << "left vel " << leftVel << "; ";
	errorFile << "right vel " << leftVel << endl;
	
	output[0] = leftVel; 
	output[1] = rightVel;
	errorFile.close();
	return output;
}


double calculateTrig(int sonarRange50Degrees)
{
	return COSONSONARSENSOR6ANGLE*sonarRange50Degrees;
}
int main(int argc, char** argv)
{
	
	Aria::init();
	ArRobot aRobot;

	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();

	ArRobotConnector robotConnector(&parser, &aRobot);
	// Try connecting to the robot.


	if (!robotConnector.connectRobot())
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
			errorFile.open("error.txt", fstream::app);
			ArSensorReading *sonarSensor[8];
			int sonarRange[8];
			for (int i = 6;i < 8;++i)
			{
				sonarSensor[i] = aRobot.getSonarReading(i);
				sonarRange[i] = sonarSensor[i]->getRange();
			}
			double* output = pid(calculateTrig(sonarRange[6]), sonarRange[7]);
			aRobot.setVel2(output[0], output[1]);
			ArUtil::sleep(100);
			errorFile.close();
		}
	}
}