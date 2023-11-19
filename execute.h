#ifndef EXECUTE_H
#define EXECUTE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>

using namespace std;

struct instruction
{
    string name;
    int rd = -1;
    int rs1 = -1;
    int rs2 = -1;
    int imm = -1;
    int is_label = 0;
    string label;
};

extern unordered_map<string, int> registers;
extern unordered_map<string, int> labelMap;
extern unordered_map<int, string> stack_pointer;
extern unordered_map<int, instruction> data_memory;
extern int PC;
extern int start_instruc_address;

int binaryToDecimal(string binary);

void executeLUI(instruction inst);
void executeAUIPC(instruction inst);
void executeJAL(instruction inst);
void executeJALR(instruction inst);
void executeBEQ(instruction inst);
void executeBNE(instruction inst);
void executeBLT(instruction inst);
void executeBGE(instruction inst);
void executeBLTU(instruction inst);
void executeBGEU(instruction inst);
void executeLB(instruction inst);
void executeLH(instruction inst);
void executeSB(instruction inst);
void executeSH(instruction inst);
void executeSW(instruction inst);
void executeSLTI(instruction inst);
void executeSLTIU(instruction inst);
void executeADDI(instruction inst);
void executeORI(instruction inst);
void executeXORI(instruction inst);
void executeANDI(instruction inst);
void executeSLLI(instruction inst);
void executeSRLI(instruction inst);
void executeSRAI(instruction inst);
void executeADD(instruction inst);
void executeSUB(instruction inst);
void executeSLL(instruction inst);
void executeSLT(instruction inst);
void executeSLTU(instruction inst);
void executeXOR(instruction inst);
void executeSRL(instruction inst);
void executeSRA(instruction inst);
void executeOR(instruction inst);
void executeAND(instruction inst);
void executeFENCE(instruction inst);
void executeECALL(instruction inst);
void executeEBREAK(instruction inst);
void executeLW(instruction inst);
void executeLHU(instruction inst);
void executeLBU(instruction inst);

void execute(vector<string> instruction);

#endif // EXECUTE_H
