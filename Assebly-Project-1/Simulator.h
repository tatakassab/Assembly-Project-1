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
	bool validate();
	void execInstructions();
	void execute(vector<string> instruction);
	void setPC(unsigned int);
	unsigned int getPC();
	void incrementPC(int);
	void incrementSP(int);
	void setRegister(string, int); //takes in a string X1, or $1, or $t1
	int getRegister(string); //takes in a string X1, or $1, or $t1
	void returnError(string); //Error handling

private:
	unsigned int PC;
	unsigned int InitialPC;
	map<unsigned int, int> Memory;
	int Registers[32];
	vector<string> rawInstructions;
	vector<vector<string>> instructions;
	map<string, unsigned int> functions;
	string file;
	string data;
	unsigned int getRelevantPC(); //this returns the index in the instructions vector
	vector<string> UType = { "LUI", "AUIPC", "JAL" };
	vector<string> OffsetIType = { "JALR", "LB", "LH", "LW", "LBU", "LHU"};
	vector<string> IType = { "ADDI", "SLTI", "SLTIU", "XORI", "ORI", "ANDI", "SLLI", "SRLI", "SRAI"};
	vector<string> SType = { "SW", "SB", "SH" };
	vector<string> SBType = { "BEQ", "BNE", "BLT", "BGE", "BLTU", "BGEU"};
	vector<string> RType = { "ADD", "SUB", "SLL", "SLT", "SLTU", "XOR", "SRL", "SRA", "OR", "AND"};
	bool isInt(string);
	bool isReg(string);
};

#endif // !SIMULATOR

