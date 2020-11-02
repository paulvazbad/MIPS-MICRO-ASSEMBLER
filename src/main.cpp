#include <iostream>
#include "MIPSParser.hpp"

int main()
{
    cout<<"RUN"<<endl;
    MIPSParser mipsParser("assembly.asm");
    mipsParser.parse();
    return 0;
}