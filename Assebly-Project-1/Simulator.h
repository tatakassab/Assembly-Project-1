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
	void output();
	string DecToHex(int output);
	string DecToBin(int output);

private:
	unsigned int PC;
	map<unsigned int, int> Memory;
	int Registers[32];
	vector<string> instructions;
	string file;
	string data;
	string Regnames[32] = { "zero", "ra", "sp", "gp", "tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6" };
};

#endif // !SIMULATOR

