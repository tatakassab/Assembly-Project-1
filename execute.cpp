include<iostream>
using namespace std;
#include "execute.h"

int binaryToDecimal(string binary)
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

void executeLUI(instruction inst){
    if (inst.rd == 0)
        {
            return;
        }
        else 
        {
            registers[inst.rd] = inst.imm << 12;
        }
}

void executeAUIPC(instruction inst){
     if (inst.rd == 0)
        {
            return;
        }
        else
        { 
            registers[inst.rd] = (inst.imm << 12)+ PC;
        }
}

void executeJAL(instruction inst){
     if(inst.rd == 0)
        {
            registers[inst.rd] = 0;
        }
        else
        {
            registers[inst.rd] = PC + 4;
        }
        if((labelMap[inst.label] - PC)  < -1048576 || (labelMap[inst.label] - PC)  > 1048575)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }  
        PC = labelMap[inst.label] - 4;
}

void executeJALR(instruction inst){
    if(inst.rd == 0)
        {
            registers[inst.rd] = 0;
        }
        else
        {
            registers[inst.rd] = inst.imm + registers[inst.rs1];
        } 
        PC = inst.imm + registers[inst.rs1] - 4;
}

void executeBEQ(instruction inst){
     if(labelMap.find(inst.label) == labelMap.end())
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1);        
        }
        if((labelMap[inst.label] - PC)  < -4096 || (labelMap[inst.label] - PC)  > 4095)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }  
        if(registers[inst.rs1] == registers[inst.rs2])
        {
            PC = labelMap[inst.label] - 4;
        }
}

void executeBNE(instruction inst){
    if(labelMap.find(inst.label) == labelMap.end())
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1);        
        }
        if((labelMap[inst.label] - PC)  < -4096 || (labelMap[inst.label] - PC)  > 4095)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }    
        if(registers[inst.rs1] != registers[inst.rs2])
        {
            PC = labelMap[inst.label] - 4;
        }
}

void executeBLT(instruction inst){
    if(labelMap.find(inst.label) == labelMap.end())
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1);        
        }
        if((labelMap[inst.label] - PC)  < -4096 || (labelMap[inst.label] - PC)  > 4095)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }
        if(registers[inst.rs1] < registers[inst.rs2])
        {
            PC = labelMap[inst.label] - 4;
        }
}

void executeBGE(instruction inst){
     if(labelMap.find(inst.label) == labelMap.end())
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1);        
        }
        if((labelMap[inst.label] - PC)  < -4096 || (labelMap[inst.label] - PC)  > 4095)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }
        if(registers[inst.rs1] >= registers[inst.rs2])
        {
            PC = labelMap[inst.label] - 4;
        }
}

void executeBLTU(instruction inst){
    if(labelMap.find(inst.label) == labelMap.end())
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1);        
        }
        if((labelMap[inst.label] - PC)  < -4096 || (labelMap[inst.label] - PC)  > 4095)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }
        if((unsigned int)registers[inst.rs1] < (unsigned int)registers[inst.rs2])
        {
            PC = labelMap[inst.label] - 4;
        }
}

void executeBGEU(instruction inst){
    if(labelMap.find(inst.label) == labelMap.end())
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1);        
        }
        if((labelMap[inst.label] - PC)  < -4096 || (labelMap[inst.label] - PC)  > 4095)
        {
            cout << "Error in line " << (PC - start_instruc_address)/4 + 1 << " (Instructions file)" << endl;
            exit(1); 
        }  
        if ((unsigned int)registers[inst.rs1] >= (unsigned int)registers[inst.rs2])  
        {
            PC = labelMap[inst.label] - 4;
        }
}

void executeLB(instruction inst){
    if(inst.rd==0)
        {
            return;
        }
        else if (inst.rs1== 2) // stack pointer 
        {
            int stack = registers[2] + (1*inst.imm);
            string val="00000000";
            string newVal;
            val.replace(0,8, stack_pointer[stack].dat_val);
            if(val[0]=='0')
            {
                newVal="000000000000000000000000";
                newVal+=val;
            }
            else if(val[0]=='1')
            {
                newVal="111111111111111111111111";
                newVal+=val;
            }
            registers[inst.rd]=binaryToDecimal(newVal);
        }
        else 
        {
            int start_add=inst.imm+ registers[inst.rs1];
            string val="00000000";
            string newVal;
            val.replace(0,8, data_memory[start_add].dat_val);
            if(val[0]=='0')
            {
                newVal="000000000000000000000000";
                newVal+=val;
            }
            else if(val[0]=='1')
            {
                newVal="111111111111111111111111";
                newVal+=val;
            }
            registers[inst.rd]=binaryToDecimal(newVal);
        }
}

void executeLH(instruction inst){
    if(inst.rd==0)
        {
            return;
        }
        else if (inst.rs1==2) // stack pointer 
        {
            int stack = registers[2] + (1*inst.imm);
            string val="0000000000000000";
            string newVal;
            val.replace(0,8, stack_pointer[stack+1].dat_val);
            val.replace(8,8, stack_pointer[stack].dat_val);
            if(val[0]=='0')
            {
                newVal="0000000000000000";
                newVal+=val;
            }
            else if(val[0]=='1')
            {
                newVal="1111111111111111";
                newVal+=val;
            }
            registers[inst.rd]=binaryToDecimal(newVal);
        }
        else 
        {
            int start_add=inst.imm+ registers[inst.rs1];
            string val="0000000000000000";
            string newVal;
            val.replace(0,8, data_memory[start_add+1].dat_val);
            val.replace(8,8, data_memory[start_add].dat_val);
            if(val[0]=='0')
            {
                newVal="0000000000000000";
                newVal+=val;
            }
            else if(val[0]=='1')
            {
                newVal="1111111111111111";
                newVal+=val;
            }
            registers[inst.rd]=binaryToDecimal(newVal);
        }
}

void executeSB(instruction inst){
    if (inst.rs1 == 2)// stack pointer
        {
            int stack = registers[2] + (1*inst.imm);
            string val = decimalToBinary(registers[inst.rd]).substr(24,8);
            stack_pointer[stack].dat_val = val.substr(0,8);
        }
        else 
        {
            int start_add = inst.imm + registers[inst.rs1];
            string val = decimalToBinary(registers[inst.rd]).substr(24,8);
            data_memory[start_add].dat_val = val.substr(0,8);
        }
}

void executeSH(instruction inst){
    if (inst.rs1 == 2) // stack pointer
        {
            int stack = registers[2] + (1*inst.imm);
            string val = decimalToBinary(registers[inst.rd]).substr(16,16);
            stack_pointer[stack+1].dat_val = val.substr(0,8);
            stack_pointer[stack].dat_val = val.substr(8,8);
        }
        else 
        {
            int start_add = inst.imm + registers[inst.rs1];
            string val = decimalToBinary(registers[inst.rd]).substr(16,16);
            data_memory[start_add+1].dat_val = val.substr(0,8);
            data_memory[start_add].dat_val = val.substr(8,8);
        }
}

void executeSW(instruction inst){
     if (inst.rs1 == 2) // stack pointer 
        {
            int stack = registers[2] + (1*inst.imm);
            string val = decimalToBinary(registers[inst.rd]).substr(0,32);
            stack_pointer[stack+3].dat_val = val.substr(0,8);
            stack_pointer[stack+2].dat_val = val.substr(8,8);
            stack_pointer[stack+1].dat_val = val.substr(16, 8);
            stack_pointer[stack].dat_val = val.substr(24,8);
        }
        else 
        {
            int start_add = inst.imm + registers[inst.rs1];
            string val = decimalToBinary(registers[inst.rd]).substr(0,32);
            data_memory[start_add+3].dat_val = val.substr(0,8);
            data_memory[start_add+2].dat_val = val.substr(8,8);
            data_memory[start_add+1].dat_val = val.substr(16, 8);
            data_memory[start_add].dat_val = val.substr(24,8);
        }
}

void executeSLTI(instruction inst){
    if(inst.rd== 0)
        {
            return;
        }
        if (registers[inst.rs1] < inst.imm)
        {
            registers[inst.rd] = 1;   // set rd = 1 if rs1 less than imm
        }
        else 
        { 
            registers[inst.rd] = 0; 
        }
}

void executeSLTIU(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        if ((unsigned int)registers[inst.rs1] < (unsigned int)inst.imm)
        {
            registers[inst.rd] = 1;   // set rd = 1 if rs1 less than imm
        }
        else 
        { 
            registers[inst.rd] = 0; 
        }
}

void executeADDI(instruction inst){
    if( inst.rd == 0)
        {
            return;
        }
        else if (inst.rd == 2 && inst.rs1 == 2) // stack pointer
        {   
            int size = inst.imm;
            if ( size < 0 ) // if we want to empty stack
            {   
                while (size != 0 )   
                {  
                    stack_pointer[registers[2] + size].dat_val = "00000000";
                    size ++;
                } 
            }
            if(size > 0) // we want to push the stack back
            {
                size = size *-1 ; // flip the size 
                while (size != 0)
                {
                    stack_pointer[registers[2] + size].dat_val = "00000000";
                    size ++;
                }
            }
            registers[2] = registers[2] + inst.imm;
        }
        else 
        {
            registers[inst.rd] = registers[inst.rs1] + inst.imm;
        }
}

void executeORI(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] | inst.imm;
}

void executeXORI(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] ^ inst.imm;
}

void executeANDI(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] & inst.imm;
}

void executeSLLI(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] << inst.imm;
}

void executeSRLI(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = (unsigned int )registers[inst.rs1] >> inst.imm;
}

void executeSRAI(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] >> inst.imm;
}

void executeADD(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] + registers[inst.rs2];
}

void executeSUB(instruction inst){
     if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] - registers[inst.rs2];
}

void executeSLL(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        registers[inst.rd] = registers[inst.rs1] << registers[inst.rs2];
}

void executeSLT(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        if (registers[inst.rs1] < registers[inst.rs2])
        {
            registers[inst.rd] = 1;   // set rd = 1 if rs1 less than rs2
        }
        else 
        { 
            registers[inst.rd] = 0; 
        }
}

void executeSLTU(instruction inst){
    if( inst.rd== 0)
        {
            return;
        }
        if ((unsigned int)registers[inst.rs1] < (unsigned int)registers[inst.rs2])
        {
            registers[inst.rd] = 1;   // set rd = 1 if rs1 less than rs2
        }
        else 
        { 
            registers[inst.rd] = 0; 
        }
}

void executeXOR(instruction inst){
    if( inst.rd== 0)
        {
            return ;
        }
        registers[inst.rd] = registers[inst.rs1] ^ registers[inst.rs2];
}

void executeSRL(instruction inst){
    if( inst.rd== 0)
        {
            return ;
        }
        registers[inst.rd] = (unsigned int )registers[inst.rs1] >> registers[inst.rs2];
}

void executeSRA(instruction inst){
    if( inst.rd== 0)
        {
            return ;
        }
        registers[inst.rd] = registers[inst.rs1] >> registers[inst.rs2];
}

void executeOR(instruction inst){
    if( inst.rd== 0)
        {
            return ;
        }
        registers[inst.rd] = registers[inst.rs1] | registers[inst.rs2];
}

void executeAND(instruction inst){
     if( inst.rd== 0)
        {
            return ;
        }
        registers[inst.rd] = registers[inst.rs1] & registers[inst.rs2];
}

void executeFENCE(instruction inst){
    riscvFormat();
        cout << "-- program is finished running (0) --" << endl;
        exit(0);
}

void executeECALL(instruction inst){
    riscvFormat();
        cout << "-- program is finished running (0) --" << endl;
        exit(0);
}

void executeEBREAK(instruction inst){
     riscvFormat();
        cout << "-- program is finished running (0) --" << endl;
        exit(0);
}

void executeLW(instruction inst){
    if(inst.rd== 0)
    {
        return;
    }
    else if (inst.rs1 == 2) // loading from  stack
    {
        int stack = registers[2] + ((1*inst.imm));
        string val = "00000000000000000000000000000000";
        val.replace(0, 8, stack_pointer[stack+3].dat_val);
        val.replace(8, 8, stack_pointer[stack+2].dat_val);
        val.replace(16, 8, stack_pointer[stack+1].dat_val);
        val.replace(24, 8, stack_pointer[stack].dat_val);
        registers[inst.rd] = binaryToDecimal(val);
    }
    else
    {
        int start_add = inst.imm + registers[inst.rs1];
        string val = "00000000000000000000000000000000";
        val.replace(0, 8, data_memory[start_add+3].dat_val);
        val.replace(8, 8, data_memory[start_add+2].dat_val);
        val.replace(16, 8, data_memory[start_add+1].dat_val);
        val.replace(24, 8, data_memory[start_add].dat_val);
        registers[inst.rd] = binaryToDecimal(val);
    }
}

void executeLHU(instruction inst){

    if( inst.rd == 0)
    {
        return;
    }
    else if (inst.rs1 == 2)// stack pointer
    {
        int stack = registers[2] + (1*inst.imm);
        string val="0000000000000000";
        val.replace(0, 8, stack_pointer[stack+1].dat_val);
        val.replace(8, 8, stack_pointer[stack].dat_val);
        string newVal="0000000000000000";
        newVal+=val;
        registers[inst.rd]= binaryToDecimal(newVal);
    }
    else
    {
        int start_add = inst.imm + registers[inst.rs1];
        string val="0000000000000000";
        val.replace(0, 8, data_memory[start_add+1].dat_val);
        val.replace(8, 8, data_memory[start_add].dat_val);
        string newVal="0000000000000000";
        newVal+=val;
        registers[inst.rd]= binaryToDecimal(newVal);
    }
}

void executeLBU(instruction inst){
    if ( inst.rd == 0 )
    {
        return;
    }
    else if (inst.rs1 == 2) // stack pointer
    {
        int stack = registers[2] + (1*inst.imm);
        string val="00000000";
        val.replace(0, 8, stack_pointer[stack].dat_val);
        string newVal="000000000000000000000000";
        newVal+=val;
        registers[inst.rd]= binaryToDecimal(newVal);
    }
    else
    {
        int start_add = inst.imm + registers[inst.rs1];
        string val="00000000";
        val.replace(0, 8, data_memory[start_add].dat_val);
        string newVal="000000000000000000000000";
        newVal+=val;
        registers[inst.rd]= binaryToDecimal(newVal);
    }
}

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

void execute(vector<string> instruction)  
{
    instruction inst;
    inst.name = instruction.at(0);
    inst.rd = instruction.at(1);
    inst.rs1 = instruction.at(2);
    inst.rs2 = instruction.at(3);
    inst.imm = instruction.at(3);
    if (inst.name == "lui")
    {
        executeLUI(inst);
    }
    if (inst.name == "auipc")
    {
       executeAUIPC(inst);
    }
    if (inst.name == "jal") 
    {
       executeJAL(inst);
    }
    if (inst.name == "jalr") 
    {
        executeJALR(inst);
    }
    if (inst.name == "beq") 
    {
       executeBEQ(inst);
    }
    if (inst.name == "bne") 
    {
        executeBNE(inst);
    }
    if (inst.name == "blt") 
    {
        executeBLT(inst);
    }
    if (inst.name == "bge") 
    {
       executeBGE(inst);
    }
    if (inst.name == "bltu")
    {
        executeBLTU(inst);
    }
    if (inst.name == "bgeu") 
    {
        executeBGEU(inst);
    }
    if (inst.name == "lb") 
    {
        executeLB(inst);
    }
    if (inst.name == "lh") 
    {
        executeLH(inst);
    }
    if (inst.name == "lw") 
    {
        executeLW(inst);
    }
    if (inst.name == "lbu") 
    {
        executeLBU(inst);
    }
    if (inst.name == "lhu") 
    {
        executeLHU(inst);
    }
    if (inst.name == "sb")
    {
        executeSB(inst);
    }
    if (inst.name == "sh")
    {
        executeSH(inst);
    }
    if (inst.name == "sw") 
    {
       executeSW(inst);
    }
    if (inst.name == "addi")
    {
        executeADDI(inst);
    }
    if (inst.name == "slti") 
    {
        executeSLTI(inst);
    }
    if (inst.name == "sltiu") 
    {
        executeSLTIU(inst);

    }
    if (inst.name == "xori") 
    {   
        executeXORI(inst);
    }
    if (inst.name == "ori")
    {   
        executeORI(inst);
    }
    if (inst.name == "andi") 
    {
        executeANDI(inst);
    }
    if (inst.name == "slli") 
    {
        executeSLLI(inst);
    }
    if (inst.name == "srli") 
    {
        executeSRLI(inst);
    }
    if (inst.name == "srai") 
    {   

        executeSRAI(inst);
    }
    if (inst.name == "add") 
    {   
        executeADD(inst);
    }
    if (inst.name == "sub") 
    {  
        executeSUB(inst);
    }
    if (inst.name == "sll") 
    {   
        executeSLL(inst);
    }
    if (inst.name == "slt")
    {
        executeSLT(inst);
    }
    if (inst.name == "sltu") 
    {
        executeSLTU(inst);
    }
    if (inst.name == "xor") 
    {
        executeXOR(inst);
    }
    if (inst.name == "srl")
    {
        executeSRL(inst);
    }
    if (inst.name == "sra") 
    {
        executeSRA(inst);
    }
    if (inst.name == "or") 
    {
        executeOR(inst);
    }
    if (inst.name == "and")
    {
       executeAND(inst);
    }
    if (inst.name == "fence") 
    {
        executeFENCE(inst);
    }
    if (inst.name == "ecall") 
    {
        executeECALL(inst);
    }
    if (inst.name == "ebreak") 
    {
       executeEBREAK(inst);
    }
}