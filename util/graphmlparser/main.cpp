#include "protobufmodelparser.h"
#include <string>
#include <iostream>
#include "datatypes.h"
int main(int argc, char **argv){
    if(argc == 2){
        std::string model_path(argv[1]);
        ProtobufModelParser parser(model_path, "experiment_id");
        std::cout << parser.GetDeploymentJSON() << std::endl;
    }
    else{
        std::cout << "Give file pls" << std::endl;
    }
};