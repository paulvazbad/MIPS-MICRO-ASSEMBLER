#ifndef MIPSPARSER_H
#define MIPSPARSER_H

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <unordered_map>
#include <sstream>
#include <regex>

using namespace std;

// Instruction types
string R_Example = "  ADD   $v1, $v0,  $at ";
string R_Example_JR = "  JR  $at";
string I_Example = "ORI $t1,$t1, 0x14";
string I_Example_Memory = "LW $v1, 0x08($zero)";
string I_Example_LUI = "LUI $t1, 0x2000";
string J_Example = "JR ETIQUETA";

// Register map
const unordered_map<string, string>
    register_map =
        {
            {"$zero", "$r0"},
            {"$at", "$r1"},
            {"$v0", "$r2"},
            {"$v1", "$r3"},
            {"$a0", "$r4"},
            {"$a1", "$r5"},
            {"$a2", "$r6"},
            {"$a3", "$r7"},
            {"$t0", "$r8"},
            {"$t1", "$r9"},
            {"$t2", "$r10"},
            {"$t3", "$r11"},
            {"$t4", "$r12"},
            {"$t5", "$r13"},
            {"$t6", "$r14"},
            {"$t7", "$r15"},
            {"$s0", "$r16"},
            {"$s1", "$r17"},
            {"$s2", "$r18"},
            {"$s3", "$r19"},
            {"$s4", "$r20"},
            {"$s5", "$r21"},
            {"$s6", "$r22"},
            {"$s7", "$r23"},
            {"$t8", "$r24"},
            {"$t9", "$r25"},
            {"$k0", "$r26"},
            {"$k1", "$r27"},
            {"$gp", "$r28"},
            {"$sp", "$r29"},
            {"$fp", "$r30"},
            {"$ra", "$r31"},

};

// R Type Opcodes
const unordered_map<string, uint8_t> R_function_codes = {
    {"add", 0x20},
    {"sub", 0x22},
    {"and", 0x24},
    {"or", 0x25},
    {"slt", 0x2A},
    {"jr", 0x08}};

// R type data structure
typedef union
{
    struct
    {
        uint32_t funct : 6,
            shamt : 5,
            rd : 5,
            rt : 5,
            rs : 5,
            opcode : 6;
    } bit;
    uint32_t value;
} R_TYPE_STRUCT;

// I type opcodes
const unordered_map<string, uint8_t> I_op_codes = {
    {"lw", 0x23},
    {"sw", 0x2b},
    {"beq", 0x04},
    {"addi", 0x08},
    {"ori", 0x0D},
    {"lui", 0x0f}};

// I type data structure
typedef union
{
    struct
    {
        uint32_t immediate : 16,
            rt : 5,
            rs : 5,
            opcode : 6;
    } bit;
    uint32_t value;
} I_TYPE_STRUCT;

// J Op code
const string J_op_code = "j";

// J type data structure
typedef union
{
    struct
    {
        uint32_t address : 26,
            opcode : 6;
    } bit;
    uint32_t value;
} J_TYPE_STRUCT;

class MIPSParser
{
public:
    MIPSParser(string filename);
    void parse();

private:
    vector<string> extractInstructionEntities(string line);
    bool is_whitespace(const string &s);
    uint32_t generateMachineCode(vector<string> instruction_fields);
    uint8_t getRegisterValue(string register_name);
    uint8_t rnumber_register(string register_name);
    uint16_t stringToHex(string string_value);
    I_TYPE_STRUCT getMemoryAccessFields(vector<string> instruction_fields);
    bool is_hex_notation(std::string const &s);

    unordered_map<string, uint32_t> tag_table;
    string filename;
};

MIPSParser::MIPSParser(string filename) : filename(filename)
{
    std::cout << "Init" << std::endl;
}

vector<string> MIPSParser::extractInstructionEntities(string line)
{
    vector<string> instruction_fields;
    string aux;
    for (int i = 0; i <= line.length(); i++)
    {
        char chr = line[i];
        if (chr == ',' || chr == ' ' || i == line.length())
        {

            if (!is_whitespace(aux))
            {
                std::transform(aux.begin(), aux.end(), aux.begin(), ::tolower);
                instruction_fields.push_back((aux));
            }

            aux.erase();
        }
        else
        {
            aux += chr;
        }
    }
    cout << "Fields" << endl;
    for (auto ele : instruction_fields)
    {
        cout << ele << " | ";
    }
    cout << endl;
    return instruction_fields;
}

void MIPSParser::parse()
{
    //TODO: Preprocess to support tags instead of constant offset addressing
    string line;
    ifstream source_assembly_code(filename);
    FILE *output_file = fopen("output.txt", "w+");

    if (source_assembly_code.is_open())
    {
        while (getline(source_assembly_code, line))
        {
            if (!line.empty())
            {
                uint32_t machineCode = generateMachineCode(extractInstructionEntities(line));
                printf("%8.8x \n", machineCode);
                fprintf(output_file, "%8.8x \n", machineCode);
            }
        }
    }
    source_assembly_code.close();
    fclose(output_file);
    //std::cout << std::showbase << std::hex << machineCode << std::endl; // Output: 0XFF

    //extractInstructionEntities(R_Example);
}

bool MIPSParser::is_whitespace(const string &str)
{
    return str.find_first_not_of(' ') == str.npos;
}

uint32_t MIPSParser::generateMachineCode(vector<string> instruction_fields)
{
    if (instruction_fields.size() < 2)
    {
        throw std::length_error("Must be at least 1 field");
    }
    const string OPCODE = instruction_fields[0];
    if (R_function_codes.find(OPCODE) != R_function_codes.end())
    {
        if (!(instruction_fields.size() == 4 || instruction_fields.size() == 2))
        {
            throw std::length_error("R type instructions must have 4 or 2 fields");
        }

        R_TYPE_STRUCT r_fields;
        r_fields.value = 0x00;
        r_fields.bit.funct = R_function_codes.at(OPCODE);
        if (OPCODE == "jr")
        {
            // edge case JR instruction
            // JR rs [R-type]
            r_fields.bit.rs = getRegisterValue(instruction_fields[1]);
            return r_fields.value;
        }

        r_fields.bit.rd = getRegisterValue(instruction_fields[1]);
        r_fields.bit.rs = getRegisterValue(instruction_fields[2]);
        r_fields.bit.rt = getRegisterValue(instruction_fields[3]);

        return r_fields.value;
    }
    else if (I_op_codes.find(OPCODE) != I_op_codes.end())
    {
        I_TYPE_STRUCT i_fields;
        i_fields.value = 0x00;
        i_fields.bit.opcode = I_op_codes.at(OPCODE);

        // Decide if memory type instruction
        if (OPCODE == "lw" || OPCODE == "sw")
        {
            if (instruction_fields.size() < 3)
            {
                throw std::length_error("I type instructions must have at least 3 fields");
            }
            i_fields = getMemoryAccessFields(instruction_fields);
        }
        else if (OPCODE == "lui")
        {
            // TODO:  LUI rt, immediate
            i_fields.bit.rt = getRegisterValue(instruction_fields[1]);
            i_fields.bit.immediate = stringToHex(instruction_fields[2]);
        }
        else
        {

            i_fields.bit.rt = getRegisterValue(instruction_fields[1]);
            i_fields.bit.rs = getRegisterValue(instruction_fields[2]);
            i_fields.bit.immediate = stringToHex(instruction_fields[3]);
        }
        return i_fields.value;
    }
    else if (OPCODE == J_op_code)
    {
    }
    else
    {
        //Invalid OPCODE
        throw std::invalid_argument(OPCODE + " not a valid OPCODE");
    }
    return 1;
}

uint8_t MIPSParser::getRegisterValue(string register_name)
{
    if (register_map.find(register_name) != register_map.end())
    {
        // With name
        return rnumber_register(register_map.at(register_name));
    }
    else
    {
        // Check if format $r##
        return rnumber_register(register_name);
    }
}

uint8_t MIPSParser::rnumber_register(string register_name)
{
    if (regex_match(register_name, regex("\\$r[0-9]+")))
    {
        int register_position;
        register_name[0] = '0';
        register_name[1] = '0';
        istringstream ss(register_name);
        ss >> register_position;
        if (register_position >= 0 && register_position <= 31)
        {
            return (uint8_t)register_position;
        }
        else
        {
            cout << "register position: " << register_position << endl;
            throw std::length_error(" Register position  " + register_name + " is not one of the 32 registers available");
        }
    }
    else
    {
        throw std::invalid_argument(" Register--" + register_name + "--Not found");
    }
}

uint16_t MIPSParser::stringToHex(string string_value)
{
    if (is_hex_notation(string_value))
    {
        return (uint16_t)std::stoul(string_value, nullptr, 16);
    }
    else
    {
        throw std::invalid_argument("Not a valid hex value: " + string_value);
    }
}

I_TYPE_STRUCT MIPSParser::getMemoryAccessFields(vector<string> instruction_fields)
{
    I_TYPE_STRUCT i_fields;
    i_fields.value = 0x00;
    i_fields.bit.opcode = I_op_codes.at(instruction_fields[0]);
    i_fields.bit.rt = getRegisterValue(instruction_fields[1]);

    // parse base offset
    string offset, base;
    bool captureBase = false;
    for (int i = 0; i < instruction_fields[2].length(); i++)
    {
        char chr = instruction_fields[2][i];
        if (chr == '(')
        {
            captureBase = true;
            continue;
        }
        else if (chr == ')')
        {
            break;
        }
        if (captureBase)
        {
            base += chr;
        }
        else
        {
            offset += chr;
        }
    }
    i_fields.bit.rs = getRegisterValue(base);
    i_fields.bit.immediate = stringToHex(offset);

    return i_fields;
}

bool MIPSParser::is_hex_notation(std::string const &s)
{
    return s.compare(0, 2, "0x") == 0 && s.size() > 2 && s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}
#endif