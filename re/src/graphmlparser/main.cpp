#include "modelparser.h"
#include <iostream>
#include <string>
int main(int argc, char **argv){
    if(argc == 2){
        try{
            std::string model_path(argv[1]);
            auto experiment = re::ModelParser::ModelParser::ParseModel(model_path, "experiment_id");
            std::cout << re::ModelParser::ModelParser::GetDeploymentJSON(*experiment) << std::endl;
            return 0;
        }catch(const std::exception& e){
            std::cerr << e.what() << std::endl;
        }
    }
    else{
        std::cout << "Specify file as parameter." << std::endl;
    }
    return 1;
}