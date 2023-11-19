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
	void output();

private:
	void getInstructions();
	bool validate();
	void execInstructions();
	void execute(vector<string> instruction);
	void setPC(unsigned int);
	unsigned int getPC();
	void incrementPC();
	void incrementSP(int);
	void setRegister(string, int); //takes in a string X1, or T1
	int getRegister(string); //takes in a string X1, or $1, or $t1
	void returnError(string); //Error handling
	string DecToHex(int output);
	string DecToBin(int output);
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
	vector<string> RegNames = { "ZERO", "RA", "SP", "GP", "TP", "T0", "T1", "T2", "S0", "S1", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9", "S10", "S11", "T3", "T4", "T5", "T6" };
	bool isInt(string);
	bool isReg(string);
	bool isFunc(string);
	unsigned int stack_upper = 8912896; //0x880000
	unsigned int stack_lower = 8388608; //0x800000
	vector<unsigned char> getBytes(int);
	int getInt(vector<unsigned char>);

	//Instructions
	int binaryToDecimal(string binary);
	void executeLUI(vector<string> inst);
	void executeAUIPC(vector<string> inst);
	void executeJAL(vector<string> inst);
	void executeJALR(vector<string> inst);
	void executeBEQ(vector<string> inst);
	void executeBNE(vector<string> inst);
	void executeBLT(vector<string> inst);
	void executeBGE(vector<string> inst);
	void executeBLTU(vector<string> inst);
	void executeBGEU(vector<string> inst);
	void executeLB(vector<string> inst);
	void executeLH(vector<string> inst);
	void executeSB(vector<string> inst);
	void executeSH(vector<string> inst);
	void executeSW(vector<string> inst);
	void executeSLTI(vector<string> inst);
	void executeSLTIU(vector<string> inst);
	void executeADDI(vector<string> inst);
	void executeORI(vector<string> inst);
	void executeXORI(vector<string> inst);
	void executeANDI(vector<string> inst);
	void executeSLLI(vector<string> inst);
	void executeSRLI(vector<string> inst);
	void executeSRAI(vector<string> inst);
	void executeADD(vector<string> inst);
	void executeSUB(vector<string> inst);
	void executeSLL(vector<string> inst);
	void executeSLT(vector<string> inst);
	void executeSLTU(vector<string> inst);
	void executeXOR(vector<string> inst);
	void executeSRL(vector<string> inst);
	void executeSRA(vector<string> inst);
	void executeOR(vector<string> inst);
	void executeAND(vector<string> inst);
	void executeLW(vector<string> inst);
	void executeLHU(vector<string> inst);
	void executeLBU(vector<string> inst);
};

#endif // !SIMULATOR

