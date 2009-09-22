/*
 *
 * Main function of the example code-1: 8-puzzle
 *
 * */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "State.h"
#include "A_Star.h"
using namespace std;

/* reading the input from file with the name specified in filename */


int
//read_inp(const char *filename, int*** array, int& sx, int& sy, int& start_x, int& start_y, int& target_x, int& target_y)
read_inp(const char *filename, Parameter& param)
{
	ifstream fin;
	fin.open(filename);
	fin >> param.size_x >> param.size_y >> param.numTargets >> param.type >> param.startSokoban.x >> param.startSokoban.y;

	Location tempBox;
	for (int i = 0; i < param.numTargets; ++i){
		fin >> tempBox.x >> tempBox.y;
		param.startBoxes.push_back(tempBox);

		fin >> tempBox.x >> tempBox.y;
		param.targetBoxes.push_back(tempBox);
	}

	param.array = new int* [param.size_x];
	for(int i=0; i<param.size_x; i++)
		(param.array)[i] = new int[param.size_y];

	for(int i = 0; i < param.size_x; i++)
		for(int j = 0; j < param.size_y; j++)
			fin >> (param.array)[i][j];

	fin.close();
	return 0;
}

void usage(){ cerr << "Usage: <exe> <input file>\n"; }

int main(int argc, char *argv[]){

	Parameter param;

	if(argc!=2){
		usage();
		exit(1);
	}

	read_inp(argv[1],param);

	for (int i = 0; i < param.size_x; ++i)
	{
		for (int j = 0; j  < param.size_y; ++j)
			cout << param.array[i][j] << " ";
		cout << endl;
	}

	/* the initial state (the root of the a-star tree) */

	State *x = new State(param.array, param);
	x->get_env()->print();

	A_Star as;
	int number_of_states;
	State *sol = as.solve(x,number_of_states);

	cout << "\nThe path followed:\n";
    for(int i=0;i<number_of_states;i++)
    	sol[i].print();

//	cout << "\nThe map locations that have been explored during A-star search are shown with X\n";
//	sol[0].get_env()->print();

	return 0;
}

