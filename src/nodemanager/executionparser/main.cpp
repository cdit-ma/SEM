#include "modelparser.h"
#include <string>
#include <iostream>
#include "datatypes.h"
int main(int argc, char **argv){

    if(argc == 2){
        std::string model_path(argv[1]);
        Graphml::ModelParser parser(model_path);
        std::cout << parser.GetDeploymentJSON() << std::endl;
    }
};