#include "environmentcontroller.h"
#include <boost/program_options.hpp>
#include <iostream>

int main(int argc, char** argv){
    std::string environment_manager_endpoint;
    std::string shutdown_experiment;


    boost::program_options::options_description desc("Environment Manager Controller Options");
    desc.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint)->required(), "TCP endpoint of Environment Manager to connect to.");
    desc.add_options()("shutdown-experiment,s", boost::program_options::value<std::string>(&shutdown_experiment), "Shutdown experiment <name>");
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
        if(vm.count("shutdown-experiment")){
            controller.ShutdownExperiment(shutdown_experiment);
        }else if(vm.count("list-experiments")){
            for(const auto& experiment_name : controller.ListExperiments()){
                std::cout << "Experiment: " << experiment_name << std::endl;
            }
        }else{
            std::cout << desc << std::endl;
            return 1;
        }
    }catch(const std::exception& ex){
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}