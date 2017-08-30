#include "idlparser.h"
#include <iostream>

int main(int argc, char** argv){
    if(argc == 2){
        std::cout << IdlParser::ParseIdl(argv[1], true) << std::endl;
    }
    return 0;
};