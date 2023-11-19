#include "Simulator.h"
#include <algorithm>
#include <sstream>

Simulator::Simulator(string i_file, string i_data)
{
	file = i_file;
	data = i_data;
	for (int i = 0; i < 32; i++) {
		Registers[i] = 0;
	}
	Memory.clear();
	rawInstructions.clear();
	instructions.clear();
	PC = 0;
	InitialPC = 0;
	getInstructions();
}

void Simulator::getInstructions()
{
	ifstream input;
	string line;

	//Gets instructions
	input.open(file);
	while (getline(input, line)) {
		rawInstructions.push_back(line);
	}
	input.close();
	if (!validate()) {
		return;
	}

	//Gets Data
	input.open(data);
	while (getline(input, line));
	{
		int split = line.find(',');
		unsigned int mem_loc = stoi(line.substr(0, split));
		int data = stoi(line.substr(split + 1, line.length()));
		Memory.insert(pair<unsigned int, int>(mem_loc, data));
	}
	input.close();
}

bool Simulator::validate()
{
	int len = rawInstructions.size();

	//set PC
	try
	{
		setPC(stoi(rawInstructions.at(0)));
		InitialPC = stoi(rawInstructions.at(0));
		if (InitialPC < 0)
		{
			throw 1;
		}
	}
	catch (const std::exception& e)
	{
		returnError("Syntax Error at line 1, make sure that it's a positive number.");
		return;
	}
	

	for (int i = 1; i < len; i++) {
		string currentInt = rawInstructions.at(i);
		if (currentInt.find(',') == string::npos) {
			currentInt.erase(remove_if(currentInt.begin(), currentInt.end(), isspace), currentInt.end());
			if (currentInt[currentInt.length() - 1] != ':') {
				returnError("Syntax Error at line " + (i + 1));
				return false;
			}
			else {
				currentInt.erase(currentInt.size() - 1);
				functions.insert(pair<string, unsigned int>(currentInt, getPC() + instructions.size()));
			}
		}
		else {

			//turn all to uppercase, remove commas and put it in a vector of words
			transform(currentInt.begin(), currentInt.end(), currentInt.begin(), ::toupper);
			currentInt.replace(currentInt.begin(), currentInt.end(), ',', ' ');
			currentInt.replace(currentInt.begin(), currentInt.end(), '(', ' ');
			currentInt.replace(currentInt.begin(), currentInt.end(), ')', ' ');
			istringstream inst(currentInt);
			string word;
			vector<string> instruction;
			while (inst >> word) {
				instruction.push_back(word);
			}
			if (instruction.size() == 0) {
				continue;
			}

			//validate instruction
			if (find(UType.begin(), UType.end(), instruction.at(0)) != UType.end()) {

			}
			else if (find(OffsetIType.begin(), OffsetIType.end(), instruction.at(0)) != OffsetIType.end()) {

			}
			else if (find(IType.begin(), IType.end(), instruction.at(0)) != IType.end()) {

			}
			else if (find(SType.begin(), SType.end(), instruction.at(0)) != SType.end()) {

			}
			else if (find(SBType.begin(), SBType.end(), instruction.at(0)) != SBType.end()) {

			}
			else if (find(RType.begin(), RType.end(), instruction.at(0)) != RType.end()) {

			}
			else {
				returnError("Instruction not supported at " + (i + 1));
				return;
			}
		}
	}
}

void Simulator::execInstructions()
{
	//Loop over all instructions
	int instructionlen = instructions.size();
	while(getRelevantPC() < instructionlen) {
		//check if terminating instruction
		if (instruction.at(0) == "ECALL" || instruction.at(0) == "EBREAK" || instruction.at(0) == "FENCE")
		{
			break;
		}
		//execute instruction
		execute(instruction);
	}
}

void Simulator::execute(vector<string> instruction)
{
	//instruction: This is an array of words having one instruction, all of them are Upper Case
	if (instruction.at(0) == "ADD") {
		//do ADD logic
	}
}
