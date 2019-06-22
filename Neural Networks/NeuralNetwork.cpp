#include "NeuralNetwork.hpp"

void NeuralNetwork::instantializeInputLayer()
{
	for(int i=0;i<INPUTNEURONCOUNT;++i)
	{
		Perceptron p;
		inputPerceptrons.push_back(p);
	}
}

void NeuralNetwork::instantializeHiddenLayer()
{
	for(int i=0;i<HIDDENNEURONCOUNT;++i)
	{
		Perceptron p;
		vector<double> weights,pwu;
		vector<Perceptron*> perceptrons;
		double biass;
		
		for(int i2=0;i2<INPUTNEURONCOUNT;++i2)
		{
			weights.push_back(0.0001*(rand()%100-49));
			pwu.push_back(0.0);
			perceptrons.push_back(&inputPerceptrons[i2]);
		}
		biass = 0.0001*(rand()%100-49);
		p.setInputs(weights,pwu,perceptrons,biass);
		hiddenPerceptrons.push_back(p);
	}
}

void NeuralNetwork::instantializeOutputLayer()
{
	for(int i=0;i<OUTPUTNEURONCOUNT;++i)
	{
		Perceptron p;
		vector<double> weights,pwu;
		vector<Perceptron*> perceptrons;
		double biass;
		for(int i2=0;i2<HIDDENNEURONCOUNT;++i2)
		{
			weights.push_back(0.0001*(rand()%100-49));
			pwu.push_back(0.0);
			perceptrons.push_back(&hiddenPerceptrons[i2]);
		}
		biass = 0.0001*(rand()%100-49);
		p.setInputs(weights,pwu,perceptrons,biass);
		outputPerceptrons.push_back(p);
	}
}

// instantiate neural network with random weights.
NeuralNetwork::NeuralNetwork()
{
	srand((unsigned int) time(NULL));
	input1Max = 0;
	input2Max = 0;
	output1Max = 0;
	output2Max = 0;
	instantializeInputLayer();
	instantializeHiddenLayer();
	instantializeOutputLayer();
	//printWeights();
}


fstream NeuralNetwork::getFile(string& name)
{
	fstream file(name);
	if(file)
	{
		return file;
	}
	else
	{
		throw runtime_error("cannot open file");
	}
}
vector<pair<double,double>> NeuralNetwork::getCSVValues(fstream &file)
{
	vector<pair<double,double>> result;
	string token1;
	string token2;
	string row1;
	string row2;
	getline(file,row1);
	getline(file,row2);
	istringstream row1stream(row1);
	istringstream row2stream(row2);
	
	while(std::getline(row1stream, token1, ',') && std::getline(row2stream, token2, ','))
	{
		result.push_back(pair<double,double>(atof(token1.c_str()),atof(token2.c_str())));
	}
	return result;
}
double NeuralNetwork::fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}


// feed forward setting input perceptron state. and then propagating forwards.
void NeuralNetwork:: generatedOutput(pair<double,double>& input)
{
	inputPerceptrons[0].setState(input.first);
	inputPerceptrons[1].setState(input.second);
	
	for(int hiddenindex = 0;hiddenindex<HIDDENNEURONCOUNT;++hiddenindex)
	{
		hiddenPerceptrons[hiddenindex].feedForward();
	}
	for(int outputindex = 0;outputindex<OUTPUTNEURONCOUNT;++outputindex)
	{
		outputPerceptrons[outputindex].feedForward();
	}
}

// get error
double NeuralNetwork::getError(double input, double state)
{
	double error = state * (1-state) * (input-state);
	return error;
}

void NeuralNetwork::printWeights()
{
	 cout << "HiddenPerceptrons: " << endl;
	 for(int index=0;index<static_cast<int>(hiddenPerceptrons.size());++index)
	 {
		 vector<double> weights = hiddenPerceptrons[index].getWeights();
		 for(double weight : weights)
		 {
			 cout << weight << endl;
		 }
	 }
	cout << "OutputPerceptrons: " << endl;
	for(int index=0;index<static_cast<int>(outputPerceptrons.size());++index)
	{
		vector<double> weights = outputPerceptrons[index].getWeights();
		for(double weight : weights)
		{
			cout << weight << endl;
		}
	}
}

void NeuralNetwork::printError(pair<double,double> output, int i)
{
	double errorLeft = outputPerceptrons[0].getState() - output.first;
	double errorRight = outputPerceptrons[1].getState() - output.second;
	if (errorLeft > 0.05)
	{
		cout << "left error " << errorLeft << endl;
		cout << "found left error above threshold" << endl;
	}
	if (errorRight > 0.05)
	{
		cout << "right error " << errorRight << endl;
		cout << "found right error above threshold" << endl;
	}
	
	
}

void NeuralNetwork::learn(vector<pair<double,double>>& inputs,vector<pair<double, double>>& outputs)
{
	for(int i=0;i<static_cast<int>(inputs.size());++i)
	{
		//cout << "Printing error after iteration " << i+1 << ":" << endl;
		generatedOutput(inputs[i]);
		backProp(outputs[i]);
		//printError(outputs[i],i);
	}
}

void NeuralNetwork::backProp(pair<double,double>& expectedOutput)
{
	vector<double> pairList;
	pairList.push_back(expectedOutput.first);
	pairList.push_back(expectedOutput.second);
	vector<double> Error;

	//printWeights();
	for(int i=0;i<OUTPUTNEURONCOUNT;++i)
	{
		Perceptron* p = &outputPerceptrons[i];
		double state = p->getState();
		double error = getError(pairList[i], state);
		Error.push_back(error);
		p->adjustWeights(error);
	}
	for(int i=0;i<HIDDENNEURONCOUNT;++i)
	{
		Perceptron* p = &hiddenPerceptrons[i];
		double state = p->getState();
		double sum=0;
		vector<double> weights;
		
		for(int i2=0;i2<OUTPUTNEURONCOUNT;++i2)
		{
			Perceptron* p2 = &outputPerceptrons[i2];
			weights.push_back(p2->getIncommingWeight(p));
		}
		for(int i3=0;i3<OUTPUTNEURONCOUNT;++i3)
		{
			sum+=weights[i3]*Error[i3];
		}
		double gradient = getError(sum+state,state);
		p->adjustWeights(gradient);
	}
	//printWeights();
}
void NeuralNetwork::checkOutput()
{
	/*
	cout << "InputPerceptrons: " << endl;
	for(int index=0;index<inputPerceptrons.size();++index)
	{
		cout << inputPerceptrons[index].getState() << endl;
	}
	cout << "HiddenPerceptrons: " << endl;
	for(int indexHidden=0;indexHidden<hiddenPerceptrons.size();++indexHidden)
	{
		cout << hiddenPerceptrons[indexHidden].getState() << endl;
	}
	cout << "OutputPerceptrons: " << endl;
	for(int indexOutput=0;indexOutput<outputPerceptrons.size();++indexOutput)
	{
		cout << outputPerceptrons[indexOutput].getState() << endl;
	}*/

}

pair<double, double> NeuralNetwork::processInput(pair<double, double>& input)
{
	generatedOutput(input); // just feeding forward and getting outputs from output perceptrons - used to get an output from feedforward
	pair<double, double> outputs;
	outputs.first = outputPerceptrons[0].getState();
	outputs.second = outputPerceptrons[1].getState();
	return outputs;
}

void NeuralNetwork::removeDuplicates(fstream& inputcsv){

    fstream outfile;
    outfile.open("../../nntrainingdatafinal.csv",ios::out|ios::trunc);
    map<string,int> mymap;
    string cur;
    while(getline(inputcsv,cur)){
        if(!mymap[cur]){
            mymap[cur]=1;
            outfile.write(cur.c_str(),cur.length());
            outfile<<endl;
        }
        else
            mymap[cur]++;
    }
    outfile.close();
}



pair<vector<pair<double,double>>,vector<pair<double,double>>> NeuralNetwork::extractFromStandardCSV(fstream& file){

		vector<pair<double,double>> result1, result2;
		string row;
		pair<pair<double,double>,pair<double,double>> data_row;
		string temp;
		while(std::getline(file,row)){
			int pos1 = row.find(',');
			temp = row.substr(0, pos1-1);
			data_row.first.first = atof(temp.c_str());

			int pos2 = row.find(',',pos1+1);
			temp = row.substr(pos1+1, pos2-1);
			data_row.first.second = atof(temp.c_str());

			int pos3 = row.find(',',pos2+1);
			temp = row.substr(pos2+1, pos3-1);
			data_row.second.first = atof(temp.c_str());

			temp = row.substr(pos3+1);
			data_row.second.second = atof(temp.c_str());

			result1.push_back(data_row.first);
			result2.push_back(data_row.second);
		}

		return pair<vector<pair<double,double>>,vector<pair<double,double>>>(result1,result2);

}

double NeuralNetwork::max_first(vector<pair<double,double>>& inputvector){

	double max=0.0;

	for(int i=0; i<static_cast<int>(inputvector.size());++i){
		if(inputvector[i].first>max){
			max=inputvector[i].first;
		}
	}

	return max;
}

double NeuralNetwork::max_second(vector<pair<double,double>>& inputvector){

	double max=0.0;

	for(int i=0; i<static_cast<int>(inputvector.size());++i){
		if(inputvector[i].second>max){
			max=inputvector[i].second;
		}
	}

	return max;
}


void NeuralNetwork::preprocessCSV(pair<vector<pair<double,double>>,vector<pair<double,double>>>& input){

	//find the max,centre the inputs between -1 and 1, and centre the outputs about 0.5 in a region where we have a decent gradient between 0.25 and 0.75

	double max_input1 = max_first(input.first);
	input1Max = max_input1;
	cout << "Input 1 max" << input1Max<<endl;
	double max_input2 = max_second(input.first);
	input2Max = max_input2;
	cout << "Input 2 max" << input2Max << endl;

	double max_output1 = max_first(input.second);
	output1Max = max_output1;
	cout << "Output 1 max" << output1Max << endl;
	double max_output2 = max_second(input.second);
	output2Max = max_output2;
	cout << "Output 2 max" << output2Max << endl;

	//centre the inputs between -1 and 1 - dividing by maximum_input multiplied by 2 because dividing by max gives a value of 0 to 1
	// multiply by 2 to get value between 0 and 2
	// subtract by 1 to give value between -1 and 1
	// purpose of preprocessing data between 0 and 1 is essential reason between .25 and .75 would always hhave a decent gradient.
	// inputs so that a decent gradient could be achieved.
	// gradient between  and 1 was tested but this was selected because gradient around 0 or 1 would be poor.
	for(int i=0; i<static_cast<int>(input.first.size());++i){
	input.first[i].first = 	2*input.first[i].first/max_input1 - 1;
	}

	for(int i=0; i<static_cast<int>(input.first.size());++i){
	input.first[i].second = 	2*input.first[i].second/max_input2 - 1;
	}

	//centre the outputs between 0.25 and 0.75

	for(int i=0; i<static_cast<int>(input.second.size());++i){
	input.second[i].first = 	0.5*input.second[i].first/max_output1 + 0.5;
	}

	for(int i=0; i<static_cast<int>(input.second.size());++i){
	input.second[i].second = 	0.5*input.second[i].second/max_output2 +0.5;
	}


}

void NeuralNetwork::randomize(pair<vector<pair<double,double>>,vector<pair<double,double>>>& input){

int size = static_cast<int>(input.first.size());

vector<int> index;

cout<<size<<endl;

for(int i=0; i<size; ++i){
	index.push_back(i);
	cout<<i<<endl;
}
srand(unsigned(time(0)));

random_shuffle(index.begin(),index.end());

pair<vector<pair<double,double>>,vector<pair<double,double>>> temp = input;

for (int i=0; i<size; ++i){
	input.first[i] = temp.first[index[i]];
	input.second[i] = temp.second[index[i]];
}
}

double NeuralNetwork::meanSquaredError(vector<pair<double,double>>& input,vector<pair<double,double>>& modelOutput){

	double sum = 0.0;

	for(int i=0;i<static_cast<int>(input.size());++i){

		generatedOutput(input[i]);

		sum += pow(modelOutput[i].first-outputPerceptrons[0].getState(),2);

		sum += pow(modelOutput[i].second-outputPerceptrons[1].getState(),2);


}

	return sum/(static_cast<double>(input.size()));



}

void NeuralNetwork::saveWeightsToFile(string filename){

	fstream outfile;
	outfile.open(filename,ios::out|ios::trunc);
	for(int index=0;index<static_cast<int>(hiddenPerceptrons.size());++index)
	{
		vector<double> weights = hiddenPerceptrons[index].getWeights();
		for(int i=0;i<static_cast<int>(weights.size())-1;++i)
		{
			outfile << weights[i] << endl;
		}

			outfile<<weights[static_cast<int>(weights.size())-1]<<endl;
			outfile<<hiddenPerceptrons[index].getBias()<<endl;
	}
	for(int index=0;index<static_cast<int>(outputPerceptrons.size());++index)
	{
		vector<double> weights = outputPerceptrons[index].getWeights();
		for(int i=0;i<static_cast<int>(weights.size())-1;++i)
		{
				outfile << weights[i] << endl;
		}

		outfile<<weights[static_cast<int>(weights.size())-1]<<endl;
		outfile<<outputPerceptrons[index].getBias()<<endl;
	}

	outfile.close();
}


void NeuralNetwork::readWeightsFromFile(string filename){

	fstream infile;

	infile.open(filename,ios::in);
	string str="";

	for(int i=0; i<HIDDENNEURONCOUNT; ++i){
		vector<double> weights;
		double bias;
		for(int j=0;j<INPUTNEURONCOUNT;++j){
			getline(infile,str);
			weights.push_back(atof(str.c_str()));
		}
		hiddenPerceptrons[i].setWeights(weights);

		getline(infile,str);
		bias = atof(str.c_str());
		hiddenPerceptrons[i].setBias(bias);
	}

	for(int i=0; i<OUTPUTNEURONCOUNT; ++i){
		vector<double> weights;
		double bias;
		for(int j=0;j<HIDDENNEURONCOUNT;++j){
			getline(infile,str);
			weights.push_back(atof(str.c_str()));
		}
		outputPerceptrons[i].setWeights(weights);

		getline(infile,str);
		bias = atof(str.c_str());
		outputPerceptrons[i].setBias(bias);

	}

	infile.close();


}

// function to remove normalisation or de-center the output.
pair<double,double> NeuralNetwork::decentredOutput(pair<double,double>& input){

if(input1Max == 0 || input2Max == 0 || output1Max == 0 || output2Max == 0){

	return processInput(input);

}
else{

	pair<double, double> dcinput;

	dcinput.first = 2*input.first/input1Max-1; // multiplied by 2 as it's multiiplied by a half in p pre process csv to de center it. divided by maximum as min is always 0

	dcinput.second = 2 * input.second/input2Max - 1; // -1 exists to restrict input so that it's between -1 and 1 similar to tanh.

	cout << dcinput.first << " " << dcinput.second << endl;

	cout << processInput(dcinput).first << " " << processInput(dcinput).second << endl;

	return pair<double,double>(2*output1Max*(processInput(dcinput).first-0.5), 2*output2Max*(processInput(dcinput).second-0.5));

}

}
