#include "Simulator.h"
#include <algorithm>
#include <sstream>

Simulator::Simulator(string i_file, string i_data)
{
	file = i_file;
	data = i_data;
	getInstructions();
}

void Simulator::getInstructions()
{
	ifstream input;
	string line;

	//Gets instructions
	input.open(file);
	while (getline(input,line)){
		instructions.push_back(line);
	}
	input.close();

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

void Simulator::execInstructions()
{
	//set PC
	PC = stoi(instructions.at(0));

	//Loop over all instructions
	int instructionlen = instructions.size();
	for (int i = 1; i < instructionlen; i++) {
		//turn all to uppercase, remove commas and put it in a vector of words
		string current_inst = instructions.at(i);
		transform(current_inst.begin(), current_inst.end(), current_inst.begin(), ::toupper);
		current_inst.replace(current_inst.begin(), current_inst.end(), ',', ' ');
		istringstream inst(current_inst);
		string word;
		vector<string> instruction;
		while (inst >> word) {
			instruction.push_back(word);
		}
		if (instruction.size() == 0) {
			continue;
		}
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
