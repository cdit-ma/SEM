#include "idlparser.h"
#include <iostream>

<<<<<<< HEAD
int main(int argc, char** argv){
    if(argc == 2){
        std::cout << IdlParser::ParseIdl(argv[1], true) << std::endl;
    }
    return 0;
};
=======
int main(int argc, char *argv[])
{
    if(argc > 1){
        auto path = std::string(argv[1]);
        auto graphml = IdlParser::IdlParser::ParseIdl(path, true);
        std::cout << graphml << std::endl;
        return graphml.size() > 0;
    }
    return 0;
}
>>>>>>> develop
