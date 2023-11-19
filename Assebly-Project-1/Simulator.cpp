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
				Memory.insert(pair<unsigned int, int>(mem_loc, data));
			}
			catch (int n) {
				returnError("Syntax Error in Data Reading, Make sure that every line consists of a number divisible by 4 and not occupied by the stack or instructions for address, then comma, then data as an integer");
				return;
			}
			catch (const std::exception&)
			{
				returnError("Syntax Error in Data Reading, Make sure that every line consists of a number divisible by 4 and not occupied by the stack or instructions for address, then comma, then data as an integer");
				return;
			}
		}
		input.close();
	}
	catch (const std::exception&)
	{
		returnError("Error opening data file");
		return;
	}

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
	catch (int n) {
		returnError("PC value must be positive");
		return false;
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
string Simulator::DecToHex(int output)
{
	string hex;
	for (int i = 0; output > 0; i++)
	{
		int remainder = output % 16;
		if (remainder <= 9)
			hex += to_string(remainder);
		else if (remainder == 10)
			hex += 'A';
		else if (remainder == 11)
			hex += 'B';
		else if (remainder == 12)
			hex += 'C';
		else if (remainder == 13)
			hex += 'D';
		else if (remainder == 14)
			hex += 'E';
		else if (remainder == 15)
			hex += 'F';

		output = output / 16;
	}
	reverse(hex.begin(), hex.end());
	return hex;
}
string Simulator::DecToBin(int output)
{
	string bin;
	for (int i = 0; output > 0; i++)
	{
		bin += to_string(output % 2);
		output = output / 2;
	}
	reverse(bin.begin(), bin.end());
	return bin;
}
void Simulator::output()
{
	cout << "The regisers values:" << endl;
	cout << left << setw(16) << "Register's Name" << setw(18) << "Register's Number" << setw(40) << "Register's value in binary" << setw(25) << "Register's value in decimal" << setw(25) << "Register's value in hexadecimal" << endl;
	cout << "_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _" << endl;
	for (int i = 0; i < 32; i++)
	{
		cout << setw(16) << RegNames[i] << left << setw(18) << "x" + to_string(i) << setw(40) << DecToBin(Registers[i]) << setw(25) << Registers[i] << setw(25) << DecToHex(Registers[i]) << endl;
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
	if (!isReg(reg)) {
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
		reg_number = find(RegNames.begin(), RegNames.end(), reg) - RegNames.begin();
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
	if (!isReg(in)) {
		returnError("Invalid Register Name used");
		return 0;
	}
	int reg_number;
	if (in[0] == 'X') {
		try
		{
			reg_number = stoi(in.substr(1, in.length() - 1));
		}
		catch (const std::exception& e)
		{
			returnError("Invalid Register Number used");
			return 0;
		}
	}
	else {
		reg_number = find(RegNames.begin(), RegNames.end(), in) - RegNames.begin();
	}

	if (reg_number > 32 || reg_number < 0) {
		returnError("Invalid Register Number");
		return 0;
	}
	return Registers[reg_number];
}

void Simulator::returnError(string err)
{
	cout << err << endl;
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
		if (isInt(in.substr(1, in.length() - 1))) {
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
	if (functions.find(in) != functions.end()) {
		return true;
	}
	return false;
}

vector<unsigned char> Simulator::getBytes(int n) {
	vector<unsigned char> bytes(4);

	bytes[3] = (n >> 24) & 0xFF;
	bytes[2] = (n >> 16) & 0xFF;
	bytes[1] = (n >> 8) & 0xFF;
	bytes[0] = (n >> 0) & 0xFF;
	return bytes;
}
int Simulator::getInt(vector<unsigned char> bytes) {
	int i = 0;
	i += bytes[0];
	i += bytes[1] << 8;
	i += bytes[2] << 16;
	i += bytes[3] << 24;
	return i;
}
void Simulator::execute(vector<string> inst)
{
	//instruction: This is an array of words having one instruction, all of them are Upper Case
	if (inst.at(0) == "LUI")
	{
		executeLUI(inst);
	}
	if (inst.at(0) == "AUIPC")
	{
		executeAUIPC(inst);
	}
	if (inst.at(0) == "JAL")
	{
		executeJAL(inst);
	}
	if (inst.at(0) == "JALR")
	{
		executeJALR(inst);
	}
	if (inst.at(0) == "BEQ")
	{
		executeBEQ(inst);
	}
	if (inst.at(0) == "BNE")
	{
		executeBNE(inst);
	}
	if (inst.at(0) == "BLT")
	{
		executeBLT(inst);
	}
	if (inst.at(0) == "BGE")
	{
		executeBGE(inst);
	}
	if (inst.at(0) == "BLTU")
	{
		executeBLTU(inst);
	}
	if (inst.at(0) == "BGEU")
	{
		executeBGEU(inst);
	}
	if (inst.at(0) == "LB")
	{
		executeLB(inst);
	}
	if (inst.at(0) == "LH")
	{
		executeLH(inst);
	}
	if (inst.at(0) == "LW")
	{
		executeLW(inst);
	}
	if (inst.at(0) == "LBU")
	{
		executeLBU(inst);
	}
	if (inst.at(0) == "LHU")
	{
		executeLHU(inst);
	}
	if (inst.at(0) == "SB")
	{
		executeSB(inst);
	}
	if (inst.at(0) == "SH")
	{
		executeSH(inst);
	}
	if (inst.at(0) == "SW")
	{
		executeSW(inst);
	}
	if (inst.at(0) == "ADDI")
	{
		executeADDI(inst);
	}
	if (inst.at(0) == "SLTI")
	{
		executeSLTI(inst);
	}
	if (inst.at(0) == "SLTIU")
	{
		executeSLTIU(inst);

	}
	if (inst.at(0) == "XORI")
	{
		executeXORI(inst);
	}
	if (inst.at(0) == "ORI")
	{
		executeORI(inst);
	}
	if (inst.at(0) == "ANDI")
	{
		executeANDI(inst);
	}
	if (inst.at(0) == "SLLI")
	{
		executeSLLI(inst);
	}
	if (inst.at(0) == "SRLI")
	{
		executeSRLI(inst);
	}
	if (inst.at(0) == "SRAI")
	{

		executeSRAI(inst);
	}
	if (inst.at(0) == "ADD")
	{
		executeADD(inst);
	}
	if (inst.at(0) == "SUB")
	{
		executeSUB(inst);
	}
	if (inst.at(0) == "SLL")
	{
		executeSLL(inst);
	}
	if (inst.at(0) == "SLT")
	{
		executeSLT(inst);
	}
	if (inst.at(0) == "SLTU")
	{
		executeSLTU(inst);
	}
	if (inst.at(0) == "XOR")
	{
		executeXOR(inst);
	}
	if (inst.at(0) == "SRL")
	{
		executeSRL(inst);
	}
	if (inst.at(0) == "SRA")
	{
		executeSRA(inst);
	}
	if (inst.at(0) == "OR")
	{
		executeOR(inst);
	}
	if (inst.at(0) == "AND")
	{
		executeAND(inst);
	}
}



int Simulator::binaryToDecimal(string binary)
{
	int decimal = 0;
	int position = 1;
	for (int i = binary.size() - 1; i >= 0; i--)
	{
		if (binary[i] == '1')
		{
			decimal += position;
		}
		position *= 2;
	}
	return decimal;
}

void Simulator::executeLUI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		setRegister(inst.at(1), imm << 12);
	}
	catch (const std::exception&)
	{
		returnError("Invalid immediate");
		return;
	}
}

void Simulator::executeAUIPC(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		setRegister(inst.at(1), (imm << 12) + PC);
	}
	catch (const std::exception&)
	{
		returnError("Invalid immediate");
		return;
	}
}

void Simulator::executeJAL(vector<string> inst) {
	setRegister(inst.at(1), getPC() + 4);
	if (functions.find(inst.at(2)) == functions.end()) {
		returnError("Calling a non-existant function");
	}
	else {
		setPC( (functions.find(inst.at(2)) -> second)-4 );
	}
}

void Simulator::executeJALR(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		setRegister(inst.at(1), imm + getRegister(inst.at(3)));
		setPC(imm + getRegister(inst.at(3)) - 4);
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}
}

void Simulator::executeBEQ(vector<string> inst) {
	if (functions.find(inst.at(3)) == functions.end()) {
		returnError("Calling a non-existant function");
		return;
	}
	if (getRegister(inst.at(1)) == getRegister(inst.at(2)))
	{
		setPC((functions.find(inst.at(3))->second) - 4);
	}
}

void Simulator::executeBNE(vector<string> inst) {
	if (functions.find(inst.at(3)) == functions.end()) {
		returnError("Calling a non-existant function");
		return;
	}
	if (getRegister(inst.at(1)) != getRegister(inst.at(2)))
	{
		setPC((functions.find(inst.at(3))->second) - 4);
	}
}

void Simulator::executeBLT(vector<string> inst) {
	if (functions.find(inst.at(3)) == functions.end()) {
		returnError("Calling a non-existant function");
		return;
	}
	if (getRegister(inst.at(1)) < getRegister(inst.at(2)))
	{
		setPC((functions.find(inst.at(3))->second) - 4);
	}
}

void Simulator::executeBGE(vector<string> inst) {
	if (functions.find(inst.at(3)) == functions.end()) {
		returnError("Calling a non-existant function");
		return;
	}
	if (getRegister(inst.at(1)) >= getRegister(inst.at(2)))
	{
		setPC((functions.find(inst.at(3))->second) - 4);
	}
}

void Simulator::executeBLTU(vector<string> inst) {
	if (functions.find(inst.at(3)) == functions.end()) {
		returnError("Calling a non-existant function");
		return;
	}
	if ((unsigned int)getRegister(inst.at(1)) < (unsigned int)getRegister(inst.at(2)))
	{
		setPC((functions.find(inst.at(3))->second) - 4);
	}
}

void Simulator::executeBGEU(vector<string> inst) {
	if (functions.find(inst.at(3)) == functions.end()) {
		returnError("Calling a non-existant function");
		return;
	}
	if ((unsigned int)getRegister(inst.at(1)) >= (unsigned int)getRegister(inst.at(2)))
	{
		setPC((functions.find(inst.at(3))->second) - 4);
	}
}

void Simulator::executeLBU(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		if (Memory.find(add - (add % 4)) == Memory.end()) {
			setRegister(inst.at(1), 0);
		}
		else {
			int word = Memory.find(add - (add % 4))->second;
			unsigned int byte = getBytes(word).at(add % 4);
			setRegister(inst.at(1), byte);
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}
	
}

void Simulator::executeLHU(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		if (add % 4 == 3) {
			unsigned int val0 = 0;
			unsigned int val1 = 0;
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				val0 = 0;
			}
			else {
				int word = Memory.find(add - (add % 4))->second;
				val0 = getBytes(word).at(3);
			}
			if (Memory.find(add + 1) == Memory.end()) {
				val1 = 0;
			}
			else {
				int word = Memory.find(add + 1)->second;
				val1 = getBytes(word).at(0) << 8;
			}
			unsigned int val = val0 + val1;
			setRegister(inst.at(1), val);
		}
		else {
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				setRegister(inst.at(1), 0);
			}
			else {
				int word = Memory.find(add - (add % 4))->second;
				vector<unsigned char> bytes = getBytes(word);
				unsigned int val = bytes.at(add % 4) + (bytes.at(add % 4) << 8);
				setRegister(inst.at(1), val);
			}
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}

}

void Simulator::executeLW(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		if (add % 4 != 0) {
			int word0 = 0;
			int word1 = 0;
			vector<unsigned char> word0b, word1b;
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				word0 = 0;
			}
			else {
				int word0 = Memory.find(add - (add % 4))->second;
			}
			if (Memory.find(add + 4 - (add%4)) == Memory.end()) {
				word1 = 0;
			}
			else {
				int word1 = Memory.find(add + 4 - (add%4))->second;
			}
			word0b = getBytes(word0);
			word1b = getBytes(word1);

			int val = 0;
			int mod = 0;
			for (int i = add % 4; i < add % 4 + 4; i++) {
				if (i < 4) {
					val += word0b[i] << (mod * 8);
				}
				else {
					val += word1b[i-4] << (mod * 8);
				}
				mod++;
			}
			setRegister(inst.at(1), val);
		}
		else {
			if (Memory.find(add) == Memory.end()) {
				setRegister(inst.at(1), 0);
			}
			else {
				int word = Memory.find(add - (add % 4))->second;
				setRegister(inst.at(1), word);
			}
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}

}

void Simulator::executeSB(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		unsigned int val = getRegister(inst.at(1)) & 0x000000FF;
		if (Memory.find(add - (add % 4)) == Memory.end()) {
			val = val << ((add % 4) * 8);
			Memory.insert(pair<unsigned int, int>(add - (add % 4), val));
		}
		else {
			int word = Memory.find(add - (add % 4))->second;
			vector<unsigned char> wordb = getBytes(word);
			wordb[add % 4] = val;
			word = getInt(wordb);
			Memory.insert(pair<unsigned int, int>(add - (add % 4), word));
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}
}

void Simulator::executeSH(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		unsigned int val = getRegister(inst.at(1)) & 0x0000FFFF;
		if (add % 4 == 3) {
			int word0 = 0;
			int word1 = 0;
			vector<unsigned char> word0b, word1b;
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				word0 = 0;
			}
			else {
				int word0 = Memory.find(add - (add % 4))->second;
			}
			if (Memory.find(add + 4 - (add % 4)) == Memory.end()) {
				word1 = 0;
			}
			else {
				int word1 = Memory.find(add + 4 - (add % 4))->second;
			}
			word0b = getBytes(word0);
			word1b = getBytes(word1);
			vector<unsigned char> valb = getBytes(val);
			word0b[3] = valb[0];
			word1b[0] = valb[1];
			Memory.insert(pair<unsigned int, int>(add - (add % 4), getInt(word0b)));
			Memory.insert(pair<unsigned int, int>(add + 4 - (add % 4), getInt(word1b)));
		}
		else {
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				val = val << ((add % 4) * 8);
				Memory.insert(pair<unsigned int, int>(add - (add % 4), val));
			}
			else {
				int word = Memory.find(add - (add % 4))->second;
				vector<unsigned char> wordb = getBytes(word);
				vector<unsigned char> valb = getBytes(val);
				wordb[add % 4] = valb[0];
				wordb[(add % 4)+1] = valb[1];
				word = getInt(wordb);
				Memory.insert(pair<unsigned int, int>(add - (add % 4), word));
			}
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}
}

void Simulator::executeSW(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		unsigned int val = getRegister(inst.at(1));
		if (add % 4 != 0) {
			int word0 = 0;
			int word1 = 0;
			vector<unsigned char> word0b, word1b;
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				word0 = 0;
			}
			else {
				int word0 = Memory.find(add - (add % 4))->second;
			}
			if (Memory.find(add + 4 - (add % 4)) == Memory.end()) {
				word1 = 0;
			}
			else {
				int word1 = Memory.find(add + 4 - (add % 4))->second;
			}
			word0b = getBytes(word0);
			word1b = getBytes(word1);
			vector<unsigned char> valb = getBytes(val);
			for (int i = add % 4; i < (add % 4) + 4; i++) {
				if (i < 4) {
					word0b[i] = valb[i - (add % 4)];
				}
				else {
					word1b[i-4] = valb[i - (add % 4)];
				}
			}
			Memory.insert(pair<unsigned int, int>(add - (add % 4), getInt(word0b)));
			Memory.insert(pair<unsigned int, int>(add + 4 - (add % 4), getInt(word1b)));
		}
		else {
			Memory.insert(pair<unsigned int, int>(add, val));
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}
}

void Simulator::executeSLTI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		if (getRegister(inst.at(2)) < imm)
		{
			setRegister(inst.at(1),1);   // set rd = 1 if rs1 less than imm
		}
		else
		{
			setRegister(inst.at(1), 0);
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeSLTIU(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		if ((unsigned int)getRegister(inst.at(2)) < imm)
		{
			setRegister(inst.at(1), 1);   // set rd = 1 if rs1 less than imm
		}
		else
		{
			setRegister(inst.at(1), 0);
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeADDI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), imm + getRegister(inst.at(2)));
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeORI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), imm | getRegister(inst.at(2)));
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeXORI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), imm ^ getRegister(inst.at(2)));
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeANDI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), imm & getRegister(inst.at(2)));
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}
void Simulator::executeSLLI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), getRegister(inst.at(2)) << imm);
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeSRLI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), (unsigned int)(getRegister(inst.at(2))) >> imm);
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeSRAI(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(3));
		setRegister(inst.at(1), getRegister(inst.at(2)) >> imm);
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediate");
	}
}

void Simulator::executeADD(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) + getRegister(inst.at(3)));
}

void Simulator::executeSUB(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) - getRegister(inst.at(3)));
}

void Simulator::executeSLL(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) << getRegister(inst.at(3)));
}

void Simulator::executeSLT(vector<string> inst) {
	if (getRegister(inst.at(2)) < getRegister(inst.at(3))) {
		setRegister(inst.at(1), 1);
	}
	else {
		setRegister(inst.at(1), 0);
	}
}

void Simulator::executeSLTU(vector<string> inst) {
	if ((unsigned int)getRegister(inst.at(2)) < (unsigned int)getRegister(inst.at(3))) {
		setRegister(inst.at(1), 1);
	}
	else {
		setRegister(inst.at(1), 0);
	}
}

void Simulator::executeXOR(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) ^ getRegister(inst.at(3)));
}

void Simulator::executeSRL(vector<string> inst) {
	setRegister(inst.at(1), (unsigned int)getRegister(inst.at(2)) >> getRegister(inst.at(3)));
}

void Simulator::executeSRA(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) >> getRegister(inst.at(3)));
}

void Simulator::executeOR(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) | getRegister(inst.at(3)));
}

void Simulator::executeAND(vector<string> inst) {
	setRegister(inst.at(1), getRegister(inst.at(2)) & getRegister(inst.at(3)));
}

void Simulator::executeLB(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		if (Memory.find(add - (add % 4)) == Memory.end()) {
			setRegister(inst.at(1), 0);
		}
		else {
			int word = Memory.find(add - (add % 4))->second;
			unsigned int byte = getBytes(word).at(add % 4);
			if (byte >> 7 == 1) {
				byte += 0xFFFFFF00;
			}
			setRegister(inst.at(1), byte);
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}

}

void Simulator::executeLH(vector<string> inst) {
	try
	{
		int imm = stoi(inst.at(2));
		unsigned int add = imm + getRegister(inst.at(3));
		if (add % 4 == 3) {
			unsigned int val0 = 0;
			unsigned int val1 = 0;
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				val0 = 0;
			}
			else {
				int word = Memory.find(add - (add % 4))->second;
				val0 = getBytes(word).at(3);
			}
			if (Memory.find(add + 1) == Memory.end()) {
				val1 = 0;
			}
			else {
				int word = Memory.find(add + 1)->second;
				val1 = getBytes(word).at(0) << 8;
			}
			unsigned int val = val0 + val1;
			if (val >> 15 == 1) {
				val += 0xFFFF0000;
			}
			setRegister(inst.at(1), val);
		}
		else {
			if (Memory.find(add - (add % 4)) == Memory.end()) {
				setRegister(inst.at(1), 0);
			}
			else {
				int word = Memory.find(add - (add % 4))->second;
				vector<unsigned char> bytes = getBytes(word);
				unsigned int val = bytes.at(add % 4) + (bytes.at(add % 4) << 8);
				if (val >> 15 == 1) {
					val += 0xFFFF0000;
				}
				setRegister(inst.at(1), val);
			}
		}
	}
	catch (const std::exception&)
	{
		returnError("Invalid Immediat");
	}

}