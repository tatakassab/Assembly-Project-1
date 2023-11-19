#include "Simulator.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include <iomanip>

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

	try
	{
		//Gets instructions
		input.open(file);
		while (getline(input, line)) {
			rawInstructions.push_back(line);
		}
		input.close();
	}
	catch (const std::exception&)
	{
		returnError("Error opening instruction file");
		return;
	}

	if (!validate()) {
		return;
	}

	try
	{
		input.open(data);
	}
	catch (const std::exception&)
	{
		returnError("Error opening data file");
		return;
	}
	//Gets Data
	while (getline(input, line));
	{
		int split = line.find(',');
		try
		{
			unsigned int mem_loc = stoi(line.substr(0, split));
			int data = stoi(line.substr(split + 1, line.length()));
			if (mem_loc % 4 != 0) {
				throw 1;
			}
			if (mem_loc <= stack_upper && mem_loc >= stack_lower) { //location in stack
				throw 1;
			}
			if (mem_loc >= PC && mem_loc < PC + (instructions.size()*4)) { //location is already occupied by an instruction
				throw 1;
			}
		}
		catch (const std::exception&)
		{
			returnError("Syntax Error in Data Reading, Make sure that every line consists of a number divisible by 4 and not occupied by the stack or instructions for address, then comma, then data as an integer");
			return;
		}
		Memory.insert(pair<unsigned int, int>(mem_loc, data));
	}
	input.close();

	setRegister("SP", stack_upper + 4);

	execInstructions();
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
		return false;
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
				transform(currentInt.begin(), currentInt.end(), currentInt.begin(), ::toupper);
				functions.insert(pair<string, unsigned int>(currentInt, getPC() + instructions.size()));
			}
		}
		else {

			//turn all to uppercase, remove commas and put it in a vector of words
			transform(currentInt.begin(), currentInt.end(), currentInt.begin(), ::toupper);
			currentInt.replace(currentInt.begin(), currentInt.end(), ',', ' ');
			currentInt.replace(currentInt.begin(), currentInt.end(), '(', ' ');
			currentInt.replace(currentInt.begin(), currentInt.end(), ')', ' ');
			currentInt.replace(currentInt.begin(), currentInt.end(), ';', ' ');
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
				if (!(instruction.size() == 3 && isReg(instruction.at(1)) && isInt(instruction.at(2)))) {
					returnError("Syntax Error at " + (i + 1));
					return false;
				}
			}
			else if (find(OffsetIType.begin(), OffsetIType.end(), instruction.at(0)) != OffsetIType.end()) {
				if (!(instruction.size() == 4 && isReg(instruction.at(1)) && isInt(instruction.at(2)) && isReg(instruction.at(3)))) {
					returnError("Syntax Error at " + (i + 1));
					return false;
				}
			}
			else if (find(IType.begin(), IType.end(), instruction.at(0)) != IType.end()) {
				if (!(instruction.size() == 4 && isReg(instruction.at(1)) && isReg(instruction.at(2)) && isInt(instruction.at(3)))) {
					returnError("Syntax Error at " + (i + 1));
					return false;
				}
			}
			else if (find(SType.begin(), SType.end(), instruction.at(0)) != SType.end()) {
				if (!(instruction.size() == 4 && isReg(instruction.at(1)) && isInt(instruction.at(2)) && isReg(instruction.at(3)))) {
					returnError("Syntax Error at " + (i + 1));
					return false;
				}
			}
			else if (find(SBType.begin(), SBType.end(), instruction.at(0)) != SBType.end()) {
				if (!(instruction.size() == 4 && isReg(instruction.at(1)) && isReg(instruction.at(2)) && isFunc(instruction.at(3)))) {
					returnError("Syntax Error at " + (i + 1));
					return false;
				}
			}
			else if (find(RType.begin(), RType.end(), instruction.at(0)) != RType.end()) {
				if (!(instruction.size() == 4 && isReg(instruction.at(1)) && isReg(instruction.at(2)) && isReg(instruction.at(3)))) {
					returnError("Syntax Error at " + (i + 1));
					return false;
				}
			}
			else if (!(instruction.at(0) == "ECALL" || instruction.at(0) == "EBREAK" || instruction.at(0) == "FENCE")) {
				returnError("Instruction not supported at " + (i + 1));
				return false;
			}

			//add instruction
			instructions.push_back(instruction);
		}
	}
	return true;
}

void Simulator::execInstructions()
{
	//Loop over all instructions
	int instructionlen = instructions.size();
	int currentI = getRelevantPC();
	while(currentI < instructionlen) {
		vector<string> instruction = instructions.at(currentI);
		//check if terminating instruction
		if (instruction.at(0) == "ECALL" || instruction.at(0) == "EBREAK" || instruction.at(0) == "FENCE")
		{
			break;
		}
		//execute instruction
		execute(instruction);
		currentI = getRelevantPC();
	}
}

void Simulator::execute(vector<string> instruction)
{
	//instruction: This is an array of words having one instruction, all of them are Upper Case
	if (instruction.at(0) == "ADD") {
		//do ADD logic
	}
}
string Simulator::DecToHex(int output)
{
	string hex;
	for (int i = 0; output > 0; i++)
	{
		hex += to_string(output % 16);
		output = output / 16;
	}
	return hex;
}
string Simulator::DecToBin(int output)
{
	string bin;
	for (int i = 0; output >0 ; i++)
	{
		bin+= to_string( output % 2);
		output = output / 2;
	}
	return bin;
}
void Simulator::output()
{
	cout << "The regisers values:" << endl;
	cout << left << setw(16) << "Register's Name" << setw(18) << "Register's Number" << setw(40) << "Register's value in binary" << setw(25) << "Register's value in decimal" << setw(25) << "Register's value in hexadecimal" << endl;
	cout << "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
	for (int i = 0; i < 32; i++)
	{
		cout << setw(16) << Regnames[i] << left << setw(18) << "x" + to_string(i) << setw(40) << DecToBin(Registers[i]) << setw(25) << Registers[i] << setw(25) << DecToHex(Registers[i]) << endl;
	}
	cout << "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
	if (Memory.empty())
		cout << "There are no values stored in the memory";
	else
	{
		cout << "The Memory values:" << endl;
		cout << left << setw(16) << "Memory Address" << setw(40) << "Memory value in binary" << setw(25) << "Memory value in decimal" << setw(25) << "Memory value in hexadecimal" << endl;
		cout << "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
		for (auto i : Memory)
			cout << left << setw(16) << i.first << setw(40) << DecToBin(i.second) << setw(25) << i.second << setw(25) << DecToHex(i.second) << endl; 
	}
	cout << "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
	cout << "The program counter: " << endl << PC << endl;;
}
void Simulator::setPC(unsigned int in)
{
	if (in < InitialPC) {
		returnError("PC is trying to access restricted address");
		return;
	}
	else {
		PC = in;
	}
}

unsigned int Simulator::getPC()
{
	return PC;
}

void Simulator::incrementPC()
{
	PC = PC + 4;
}

void Simulator::incrementSP(int i)
{
	unsigned int new_val = getRegister("SP") + i;
	if (new_val > stack_upper + 4) {
		returnError("Stack Pointer is trying to access restricted address");
		return;
	}
	else if (new_val < stack_lower) {
		returnError("Stack Overflow");
		return;
	}
	else {
		setRegister("SP", new_val);
	}
}

void Simulator::setRegister(string reg, int val)
{
	if (!isReg(string)) {
		returnError("Invalid Register Name used");
		return;
	}
	int reg_number;
	if (reg[0] == 'X') {
		try
		{
			reg_number = stoi(reg.substr(1, reg.length() - 1));
		}
		catch (const std::exception& e)
		{
			returnError("Invalid Register Number used");
			return;
		}
	}
	else {
		reg_number = find(RegNames.begin(), RegNames.end(), reg) - find.begin();
	}

	if (reg_number > 32 || reg_number < 0) {
		returnError("Invalid Register Number");
		return;
	}

	if (reg_number != 0) {
		Registers[reg_number] = val;
	}
	return;
}

int Simulator::getRegister(string in)
{
	if (!isReg(string)) {
		returnError("Invalid Register Name used");
		return 0;
	}
	int reg_number;
	if (reg[0] == 'X') {
		try
		{
			reg_number = stoi(reg.substr(1, reg.length() - 1));
		}
		catch (const std::exception& e)
		{
			returnError("Invalid Register Number used");
			return 0;
		}
	}
	else {
		reg_number = find(RegNames.begin(), RegNames.end(), reg) - find.begin();
	}

	if (reg_number > 32 || reg_number < 0) {
		returnError("Invalid Register Number");
		return 0;
	}
	return Registers[reg_number];
}

void Simulator::returnError(string err)
{
	cout << err << "\n".
}

unsigned int Simulator::getRelevantPC()
{
	return (PC - InitialPC) / 4;
}

bool Simulator::isInt(string in)
{
	int i = 0;
	if (in[i] == '-') {
		i++;
	}
	for (i; i < in.length(); i++) {
		if (!isdigit(in[i])) {
			return false;
		}
	}
	return true;
}

bool Simulator::isReg(string in)
{
	if (in == "") {
		return false;
	}
	if (find(RegNames.begin(), RegNames.end(), in) != RegNames.end()) {
		return true;
	}
	if (in.length() < 4 && in[0] == 'X') {
		if (isInt(in.substr(1, in.length() - 1)) {
			int x = stoi((in.substr(1, in.length() - 1)));
			if (x < 32 && x >= 0) {
				return true;
			}
		}
	}
	return false;
}


bool Simulator::isFunc(string in)
{
	if (find(functions.begin(), functions.end(), in) != functions.end()) {
		return true;
	}
	return false;
}
