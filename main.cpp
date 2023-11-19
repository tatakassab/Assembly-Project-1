#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cctype>
using namespace std;

long long int instruc_address;
long long int start_instruc_address;
int registers[32] = {0,0,12000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
string registers_names[32] = { "zero", "ra", "sp", "gp", "tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6" };
long long int PC;
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
struct data_val
{
    string dat_val = "00000000";
};
map<long long int, instruction> instruction_memory;
map<long long int, data_val> data_memory;
map<string, long long int> labelMap;
map<long long int, data_val> stack_pointer;

string decimalToBinary(int num)
{
    string binary;
    for (int i = 31; i >= 0; i--)
    {
        int bit = (num >> i) & 1;
        binary += to_string(bit);
    }
    return binary;
}

int check_input_val(string in_val)
{
    if(in_val.size() == 1 && !isdigit(in_val[0]))
    {
        return 1; //if character
    }
    else if(in_val.size() == 1 && isdigit(in_val[0]))
    {
        return 0;  //is integer
    }
    else if(in_val.size() > 1)
    {
        if(in_val[0] == '-')
        {
            for(int i = 1; i < in_val.size(); i++)
            {
                if(!isdigit(in_val[i]))
                {
                    return -1;  //invalid
                }
            }
            return 0;  //is integer
        }
        else
        {
            for(int i = 0; i < in_val.size(); i++)
            {
                if(!isdigit(in_val[i]))
                {
                    return -1;  //invalid
                }
            }
            return 0;  //is integer
        }
    }
    else
    {
        return -1;  //invalid
    }
}

void reading_initial()
{
    int line_num = 0;
    try
    {
        //reading start program address
        cout << "Please enter the program starting address: ";
        cin >> instruc_address;
        while(instruc_address < 0)
        {
            cout << "Invalid address! Please enter the program starting address: ";
            cin >> instruc_address;
        }
        start_instruc_address = instruc_address;
        PC = instruc_address;
        //reading intial data from the file
        ifstream input_file("data.txt");
        string line;
        while (getline(input_file, line)) 
        {
            long long int key = stoll(line.substr(0, line.find(",")));
            string value = line.substr(line.find(",") + 1);
            int check = check_input_val(value);
            if(check == 1)
            {
                char character = value[0];
                data_memory[key].dat_val = decimalToBinary((int)character).substr(24, 8);
            }
            else if(check == 0)
            {
                string val = decimalToBinary(stoi(value)).substr(0,32);
                data_memory[key+3].dat_val = val.substr(0,8);
                data_memory[key+2].dat_val = val.substr(8,8);
                data_memory[key+1].dat_val = val.substr(16, 8);
                data_memory[key].dat_val = val.substr(24,8);
            }
            else
            {
                throw "Error";
            }
            line_num++;
        }
        input_file.close();
    }
    catch(...)
    {
        cout << "Error in line " << line_num+ 1 << " (Data file)" << endl;
        exit(1);
    }
}


instruction convert_riscv_instruction(string riscv_instr)
{
    try
    {
        instruction instr;
        instr.rd = instr.rs1 = instr.rs2 = instr.imm = -1;
        instr.is_label = 0;

        stringstream ss(riscv_instr);
        vector<string> tokens;
        string token;
        while (ss >> token)
        {
            tokens.push_back(token);
        }

        int expected_commas = -1;
        string opcode = tokens[0];
        if (opcode == "lui" || opcode == "auipc" || opcode == "jalr" || opcode == "jal")
        {
            expected_commas = 1;
        }
        else if (opcode == "beq" || opcode == "bne" || opcode == "blt" || opcode == "bge" || opcode == "bltu" || opcode == "bgeu")
        {
            expected_commas = 2;
        }
        else if (opcode == "lb" || opcode == "lh" || opcode == "lw" || opcode == "lbu" || opcode == "lhu" || opcode == "sb" || opcode == "sh" || opcode == "sw")
        {
            expected_commas = 1;
        }
        else if (opcode == "addi" || opcode == "slti" || opcode == "sltiu" || opcode == "xori" || opcode == "ori" || opcode == "andi" || opcode == "slli" || opcode == "srli" || opcode == "srai")
        {
            expected_commas = 2;
        }
        else if (opcode == "add" || opcode == "sub" || opcode == "sll" || opcode == "slt" || opcode == "sltu" || opcode == "xor" || opcode == "srl" || opcode == "sra" || opcode == "or" || opcode == "and")
        {
            expected_commas = 2;
        }
        else if (opcode == "fence" || opcode == "ecall" || opcode == "break")
        {
            expected_commas = 0;
        }
        else
        {
            expected_commas = 0;
        }

        int actual_commas = count(riscv_instr.begin(), riscv_instr.end(), ',');
        if (actual_commas != expected_commas)
        {
            throw "Error: Invalid syntax";
        }

        // decode instruction based on opcode
        if (opcode == "lui")
        {
            if (tokens.size() != 3 || tokens[1][0] != 'x')
                throw "Error: Invalid syntax for lui instruction";
            instr.name = "lui";
            instr.rd = stoi(tokens[1].substr(1));
            string immed = tokens[2];
            if (immed[0] == '-')
            {
                for(int q = 1; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            else
            {
                for(int q = 0; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            instr.imm = stoi(immed);
            if(instr.imm < -2048 || instr.imm > 1048575)
                throw "Error: out of range for immediate value";
            if(instr.rd < 0 || instr.rd > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "auipc")
        {
            if (tokens.size() != 3 || tokens[1][0] != 'x')
                throw "Error: Invalid syntax for auipc instruction";
            instr.name = "auipc";
            instr.rd = stoi(tokens[1].substr(1));
            string immed = tokens[2];
            if (immed[0] == '-')
            {
                for(int q = 1; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            else
            {
                for(int q = 0; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            instr.imm = stoi(immed);
            if(instr.imm < -2048 || instr.imm > 1048575)
                throw "Error: out of range for immediate value";
            if(instr.rd < 0 || instr.rd > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "jal")
        {
            if (tokens.size() != 3 || tokens[1][0] != 'x')
                throw "Error: Invalid syntax for jal instruction";
            instr.name = "jal";
            instr.rd = stoi(tokens[1].substr(1));
            instr.label = tokens[2];
            if(instr.rd < 0 || instr.rd > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "jalr")
        {
            if (tokens.size() != 3 || tokens[1][0] != 'x' || tokens[2].back() != ')' || tokens[2].find('(') == string::npos || tokens[2].find('(') == 0)
                throw "Error: Invalid syntax for load/store instruction";
            instr.name = opcode;
            instr.rd = stoi(tokens[1].substr(1));
            string immed = tokens[2].substr(0, tokens[2].find('('));
            if (immed[0] == '-')
            {
                for(int q = 1; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            else
            {
                for(int q = 0; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            instr.imm = stoi(immed);
            if(instr.imm < -2048 || instr.imm > 2047)
                throw "Error: out of range for immediate value";
            string base_reg_str = tokens[2].substr(tokens[2].find('(') + 1);
            base_reg_str.pop_back(); // Remove the closing parenthesis
            instr.rs1 = stoi(base_reg_str.substr(1));
            if(instr.rd < 0 || instr.rs1 < 0 || instr.rd > 31 || instr.rs1 > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "beq" || opcode == "bne" || opcode == "blt" || opcode == "bge" || opcode == "bltu" || opcode == "bgeu")
        {
            if (tokens.size() != 4 || tokens[1][0] != 'x' || tokens[2][0] != 'x')
                throw "Error: Invalid syntax for branch instruction";
            instr.name = opcode;
            instr.rs1 = stoi(tokens[1].substr(1));
            instr.rs2 = stoi(tokens[2].substr(1));
            instr.label = tokens[3];
            if(instr.rs2 < 0 || instr.rs1 < 0 || instr.rs2 > 31 || instr.rs1 > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "lb" || opcode == "lh" || opcode == "lw" || opcode == "lbu" || opcode == "lhu" || opcode == "sb" || opcode == "sh" || opcode == "sw")
        {
            if (tokens.size() != 3 || tokens[1][0] != 'x' || tokens[2].back() != ')' || tokens[2].find('(') == string::npos || tokens[2].find('(') == 0)
                throw "Error: Invalid syntax for load/store instruction";
            instr.name = opcode;
            instr.rd = stoi(tokens[1].substr(1));
            string immed = tokens[2].substr(0, tokens[2].find('('));
            if (immed[0] == '-')
            {
                for(int q = 1; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            else
            {
                for(int q = 0; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            instr.imm = stoi(immed);
            if((opcode == "lw" || opcode == "sw") && instr.imm%4 != 0)
                throw "Error: Invalid syntax for load/store instruction";
            if((opcode == "lh" || opcode == "lhu" || opcode == "sh") && instr.imm%2 != 0)
                throw "Error: Invalid syntax for load/store instruction";
            if(instr.imm < -2048 || instr.imm > 2047)
                throw "Error: out of range for immediate value";
            string base_reg_str = tokens[2].substr(tokens[2].find('(') + 1);
            base_reg_str.pop_back(); // Remove the closing parenthesis
            instr.rs1 = stoi(base_reg_str.substr(1));
            if(instr.rd < 0 || instr.rs1 < 0 || instr.rd > 31 || instr.rs1 > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "addi" || opcode == "slti" || opcode == "sltiu" || opcode == "xori" || opcode == "ori" || opcode == "andi" || opcode == "slli" || opcode == "srli" || opcode == "srai")
        {
            if (tokens.size() != 4 || tokens[1][0] != 'x' || tokens[2][0] != 'x')
                throw "Error: Invalid syntax for immediate instruction";
            instr.name = opcode;
            instr.rd = stoi(tokens[1].substr(1));
            instr.rs1 = stoi(tokens[2].substr(1));
            string immed = tokens[3];
            if(immed[0] == '-')
            {
                for(int q = 1; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            else
            {
                for(int q = 0; q < immed.size(); q++)
                {
                    if (!isdigit(immed[q]))
                        throw "Error: Invalid syntax";
                }
            }
            instr.imm = stoi(immed);
            if(instr.imm < -2048 || instr.imm > 2047)
                throw "Error: out of range for immediate value";
            if(instr.rd < 0 || instr.rs1 < 0 || instr.rd > 31 || instr.rs1 > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "add" || opcode == "sub" || opcode == "sll" || opcode == "slt" || opcode == "sltu" || opcode == "xor" || opcode == "srl" || opcode == "sra" || opcode == "or" || opcode == "and")
        {
            if (tokens.size() != 4 || tokens[1][0] != 'x' || tokens[2][0] != 'x' || tokens[3][0] != 'x')
                throw "Error: Invalid syntax for register-register instruction";
            instr.name = opcode;
            instr.rd = stoi(tokens[1].substr(1));
            instr.rs1 = stoi(tokens[2].substr(1));
            instr.rs2 = stoi(tokens[3].substr(1));
            if(instr.rd < 0 || instr.rs1 < 0 || instr.rs2 < 0 || instr.rd > 31 || instr.rs1 > 31 || instr.rs2 > 31)
                throw "Error: Invalid syntax for register-register instruction";
        }
        else if (opcode == "fence")
        {
            if (tokens.size() != 1)
                throw "Error: Invalid syntax for fence instruction";
            instr.name = "fence";
        }
        else if (opcode == "ecall" || opcode == "ebreak")
        {
            if (tokens.size() != 1)
                throw "Error: Invalid syntax for ecall/ebreak instruction";
            instr.name = opcode;
        }
        else
        {
            if(tokens.size() != 1 || tokens[0].size() <= 1 || tokens[0].back() != ':' || count(tokens[0].begin(), tokens[0].end(), ':') != 1)
                throw "Error: Invalid syntax for instruction";
            instr.label = opcode;
            instr.label.erase(instr.label.size() - 1, 1);
            instr.is_label = 1;
            labelMap[instr.label] = instruc_address;
        }
        return instr;
    }
    catch(...)
    {
        cout << "Error in line " << (instruc_address - start_instruc_address)/4 + 1 << " (Instructions file)" <<  endl;
        exit(1);
    }
}


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

bool isWordBoundary(char c) 
{
    return !isalnum(c) && c != '_';
}

void replace_reg(string& inputString)
{
    unordered_map<string, string> wordReplacements =
    {
        {"zero", "x0"},
        {"ra", "x1"},
        {"sp", "x2"},
        {"gp", "x3"},
        {"tp", "x4"},
        {"t0", "x5"},
        {"t1", "x6"},
        {"t2", "x7"},
        {"s0", "x8"},
        {"fp", "x8"},
        {"s11", "x27"},
        {"s10", "x26"},
        {"s1", "x9"},
        {"a0", "x10"},
        {"a1", "x11"},
        {"a2", "x12"},
        {"a3", "x13"},
        {"a4", "x14"},
        {"a5", "x15"},
        {"a6", "x16"},
        {"a7", "x17"},
        {"s2", "x18"},
        {"s3", "x19"},
        {"s4", "x20"},
        {"s5", "x21"},
        {"s6", "x22"},
        {"s7", "x23"},
        {"s8", "x24"},
        {"s9", "x25"},
        {"t3", "x28"},
        {"t4", "x29"},
        {"t5", "x30"},
        {"t6", "x31"}
    };

    for (size_t i = 0; i < inputString.length(); ++i) 
    {
        if (isalpha(inputString[i]) || inputString[i] == '_') 
        {
            size_t wordLength = 0;
            for (size_t j = i; j < inputString.length(); ++j) 
            {
                if (isalnum(inputString[j]) || inputString[j] == '_') 
                {
                    ++wordLength;
                } 
                else 
                {
                    break;
                }
            }

            string word = inputString.substr(i, wordLength);

            if (wordReplacements.count(word) > 0) 
            {
                if ((i == 0 || isWordBoundary(inputString[i - 1])) && (i + wordLength == inputString.length() || isWordBoundary(inputString[i + wordLength]))) 
                {
                    inputString.replace(i, wordLength, wordReplacements[word]);

                    i += wordReplacements[word].length() - 1;
                }
            }
        }
    }
}

void readInstructions(const string& filename)
{
    ifstream infile(filename);

    if (!infile.is_open())
    {
        cerr << "Error opening file " << filename << endl;
        return;
    }

    string line_b;
    while (getline(infile  >> ws, line_b))
    {
        // Ignore empty lines and lines starting with a '#' character
        if (line_b.empty() || line_b[0] == '#')
        {
            continue;
        }
        // Remove any comment text that follows a '#' character
        size_t comment_pos = line_b.find('#');
        if (comment_pos != string::npos)
        {
            line_b = line_b.substr(0, comment_pos);
        }
        replace_reg(line_b);
        instruction line = convert_riscv_instruction(line_b);
        if(!line.is_label)
        {
            instruction_memory.insert(make_pair(instruc_address, line));
            instruc_address += 4;
        }
    }

    infile.close();
}

string to_hex(int reg)
{
    stringstream ss;
    ss << hex << reg;
    return ss.str();
}

void printMemoryData() 
{
    if(data_memory.empty())
    {
        cout << "Data memory is empty" << endl;
        return;
    }
    cout << "Content of data memory: " << endl;
    for (auto const& [key, value] : data_memory) 
    {
        cout << "Address: " << key << " | Value: " << value.dat_val << endl;
    }
}

void riscvFormat()
{
    cout << "____________________________________________________________________________________________________________" << endl;
    cout << left << setw(7)<<"Name" << setw(10) << "Number" << setw(40) <<  "Value in binary" << setw(25) << "Value in decimal" << setw(25) << "Value in hexadecimal" << endl;
    cout << "____________________________________________________________________________________________________________" << endl;
    for (int i = 0; i < 32; i++)
    {
        cout << setw(7) <<registers_names[i] << left << setw(10) << "x" + to_string(i) << setw(40) << decimalToBinary(registers[i]) << setw(25) << registers[i] << setw(25) << to_hex(registers[i]) << endl;
    }
    cout << endl << endl;
    printMemoryData();
    cout << endl << endl;
    cout << "The program counter is: " << PC << endl << endl;
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
void execute(instruction inst)  // to call the function of an insturction based on its name
{
    if (inst.name == "lui") // 1
    {
        executeLUI(inst);
    }
    if (inst.name == "auipc") // 2
    {
       executeAUIPC(inst);
    }
    if (inst.name == "jal") // 3
    {
       executeJAL(inst);
    }
    if (inst.name == "jalr")  // 4
    {
        executeJALR(inst);
    }
    if (inst.name == "beq") // 5
    {
       executeBEQ(inst);
    }
    if (inst.name == "bne") // 6
    {
        executeBNE(inst);
    }
    if (inst.name == "blt") // 7
    {
        executeBLT(inst);
    }
    if (inst.name == "bge") // 8
    {
       executeBGE(inst);
    }
    if (inst.name == "bltu") // 9
    {
        executeBLTU(inst);
    }
    if (inst.name == "bgeu") // 10
    {
        executeBGEU(inst);
    }
    if (inst.name == "lb") // 11
    {
        executeLB(inst);
    }
    if (inst.name == "lh") // 12
    {
        executeLH(inst);
    }
    if (inst.name == "lw") // 13
    {
        executeLW(inst);
    }
    if (inst.name == "lbu") // 14
    {
        executeLBU(inst);
    }
    if (inst.name == "lhu") // 15
    {
        executeLHU(inst);
    }
    if (inst.name == "sb") // 16
    {
        executeSB(inst);
    }
    if (inst.name == "sh") // 17
    {
        executeSH(inst);
    }
    if (inst.name == "sw") // 18
    {
       executeSW(inst);
    }
    if (inst.name == "addi") // 19
    {
        executeADDI(inst);
    }
    if (inst.name == "slti") // 20
    {
        executeSLTI(inst);
    }
    if (inst.name == "sltiu") // 21
    {
        executeSLTIU(inst);

    }
    if (inst.name == "xori") // 22
    {   
        executeXORI(inst);
    }
    if (inst.name == "ori") // 23
    {   
        executeORI(inst);
    }
    if (inst.name == "andi") // 24
    {
        executeANDI(inst);
    }
    if (inst.name == "slli") // 25
    {
        executeSLLI(inst);
    }
    if (inst.name == "srli") // 26
    {
        executeSRLI(inst);
    }
    if (inst.name == "srai") // 27
    {   

        executeSRAI(inst);
    }
    if (inst.name == "add") // 28
    {   
        executeADD(inst);
    }
    if (inst.name == "sub") // 29
    {  
        executeSUB(inst);
    }
    if (inst.name == "sll") // 30
    {   
        executeSLL(inst);
    }
    if (inst.name == "slt") // 31
    {
        executeSLT(inst);
    }
    if (inst.name == "sltu") // 32
    {
        executeSLTU(inst);
    }
    if (inst.name == "xor") // 33
    {
        executeXOR(inst);
    }
    if (inst.name == "srl") //34
    {
        executeSRL(inst);
    }
    if (inst.name == "sra") //35
    {
        executeSRA(inst);
    }
    if (inst.name == "or") // 36
    {
        executeOR(inst);
    }
    if (inst.name == "and") // 37
    {
       executeAND(inst);
    }
    if (inst.name == "fence") // 38
    {
        executeFENCE(inst);
    }
    if (inst.name == "ecall") // 39
    {
        executeECALL(inst);
    }
    if (inst.name == "ebreak") // 40
    {
       executeEBREAK(inst);
    }
}


void run_code()
{
    int option;
    cout << "Please type 1 or 2: " << endl;
    cout << "1) Showing the registers, data, and PC values after the execution of each line" << endl;
    cout << "2) Showing the registers, data, and PC values at the end only" << endl;
    cout << "Option: ";
    cin >> option;
    // Print out the input.
    while(option != 1 && option != 2)
    {
        cout << "Invalid input!" << endl;
        cout << "Option: ";
        cin >> option;
    }
    // Read instructions from instructions. txt and run the program
    if(option == 1)
    {
        reading_initial();
        string filename = "instructions.txt";
        readInstructions(filename);
        // Execute the instruction memory at the instruction memory address PC.
        while(PC < instruc_address)
        {
            execute(instruction_memory[PC]);
            PC += 4;
            riscvFormat();
        }
        cout << "-- program is finished running (dropped off bottom) --" << endl;
    }
    // if option 2 read instructions. txt from file
    else if(option == 2)
    {
        reading_initial();
        string filename = "instructions.txt";
        readInstructions(filename);
        // Execute the instruction at the instruction memory address PC
        while(PC < instruc_address)
        {
            execute(instruction_memory[PC]);
            PC += 4;
        }
        riscvFormat();
        cout << "-- program is finished running (dropped off bottom) --" << endl;   
    }
}

int main()
{
    run_code();
    return 0;
}