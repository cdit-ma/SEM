#include "environmentcontroller.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <google/protobuf/util/json_util.h>

int main(int argc, char** argv){
    std::string environment_manager_endpoint;
    std::string experiment_name;
    std::string graphml_path;


    boost::program_options::options_description desc("Environment Manager Controller Options");
    desc.add_options()("experiment-name,n", boost::program_options::value<std::string>(&experiment_name), "Name of experiment.");
    desc.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint)->required(), "TCP endpoint of Environment Manager to connect to.");
    desc.add_options()("shutdown-experiment,s", "Shutdown experiment <name>");
    desc.add_options()("add-experiment,a", boost::program_options::value<std::string>(&graphml_path), "Deployment graphml file path.");
    desc.add_options()("list-experiments,l", "List running experiments.");
    desc.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
    }catch(const boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }
    if(vm.count("help")){
        std::cout << desc << std::endl;
        return 0;
    }

    EnvironmentManager::EnvironmentController controller(environment_manager_endpoint);
    
    try{
        if(vm.count("shutdown-experiment") && vm.count("experiment-name")){
            controller.ShutdownExperiment(experiment_name);
        }else if(vm.count("add-experiment") && vm.count("experiment-name")){
            auto result = controller.AddExperiment(experiment_name, graphml_path);
            std::string output;
            google::protobuf::util::JsonOptions options;
            options.add_whitespace = true;

            if(result){
                google::protobuf::util::MessageToJsonString(*result, &output, options);
                std::cout << output << std::endl;
            }else{
                return 1;
            }
        }else if(vm.count("list-experiments")){
            const auto& experiment_names = controller.ListExperiments();
            std::cout << "{\"experiment_names\":[" << std::endl;
            for(int i = 0; i < experiment_names.size(); i++){
                std::cout << "\t" << "\"";
                std::cout << experiment_names.at(i);
                std::cout << "\"";
                if(i != experiment_names.size() - 1){
                    std::cout << ",";
                }
                std::cout << std::endl;
            }
            std::cout << "]}" << std::endl;
        }else{
            std::cout << desc << std::endl;
            return 1;
        }
    }catch(const std::exception& ex){
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}