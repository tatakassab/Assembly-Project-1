#pragma once
#ifndef SIMULATOR
#define SIMULATOR

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

class Simulator
{
public:
	Simulator(string i_file, string i_data);
	void getInstructions();
	void execInstructions();
	void execute(vector<string> instruction);

private:
	unsigned int PC;
	map<unsigned int, int> Memory;
	int Registers[32];
	vector<string> instructions;
	string file;
	string data;
};

#endif // !SIMULATOR

