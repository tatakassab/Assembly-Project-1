#include "Simulator.h"

int main() {
	string instructions = "instructions.txt";
	string data = "data.txt";
	Simulator simulate(instructions, data);
	return 0;
}