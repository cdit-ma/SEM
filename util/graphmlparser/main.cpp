#include "protobufmodelparser.h"
#include <string>
#include <iostream>
int main(int argc, char **argv){
    if(argc == 2){
        std::string model_path(argv[1]);
        auto experiment = ProtobufModelParser::ParseModel(model_path, "experiment_id");
        std::cout << ProtobufModelParser::GetDeploymentJSON(*experiment) << std::endl;
    }
    else{
        std::cout << "Specify file as parameter." << std::endl;
    }
};