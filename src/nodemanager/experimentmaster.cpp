#include <iostream>
#include <vector>
#include <string>
#include <signal.h>
#include <boost/program_options.hpp>

#include "executionmanager.h"

#include "cmakevars.h"
#include <util/execution.hpp>

#include "zmq/registrar.h"

std::string VERSION_NAME = "experiment_master";

Execution* exe = 0;

void signal_handler(int sig)
{
    exe->Interrupt();
}

int main(int argc, char **argv){
    //Connect the SIGINT/SIGTERM signals to our handler.
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    exe = new Execution();

    //Get the library path from the argument variables
    std::string graphml_path;
    std::string master_endpoint;
    std::string environment_manager_endpoint;
    std::string experiment_id;
    double execution_duration = 60.0;
    bool live_logging = false;

    boost::program_options::options_description options("Node manager options");
    options.add_options()("deployment,d", boost::program_options::value<std::string>(&graphml_path), "Deployment graphml file path");
    options.add_options()("time,t", boost::program_options::value<double>(&execution_duration)->default_value(execution_duration), "Deployment Duration (In Seconds)");
    options.add_options()("master,m", boost::program_options::value<std::string>(&master_endpoint), "Master endpoint, including port");
    options.add_options()("live-logging,L", boost::program_options::value<bool>(&live_logging), "Master endpoint, including port");
    options.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint), "Environment manager endpoint.");
    options.add_options()("experiment-id,n", boost::program_options::value<std::string>(&experiment_id), "ID of experiment to start.");
    options.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);
        boost::program_options::notify(vm);
    }catch(boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << options << std::endl;
        return 1;
    }

    //display help
    if(vm.count("help")){
        std::cout << options << std::endl;
        return 0;
    }

    bool valid_args = true;

    if(master_endpoint.empty()){
        std::cerr << "Arg Error: experiment_master requires a valid endpoint." << std::endl;
        valid_args = false;
    }
    if(graphml_path.empty()){
        std::cerr << "Arg Error: experiment_master requires a graphml deployment to execute." << std::endl;
        valid_args = false;
    }

    if(!valid_args){
        std::cout << options << std::endl;
        return 1;
    }

    std::cout << "-------[" + VERSION_NAME +" v" + RE_VERSION + "]-------" << std::endl;
    std::cout << "* Master:" << std::endl;
    std::cout << "** Endpoint: " << master_endpoint << std::endl;
    std::cout << "** Deployment Graphml: " << graphml_path << std::endl;

    if(execution_duration == -1){
        std::cout << "** Duration: " << "Indefinite" << std::endl;
    }else{
        std::cout << "** Duration: " << execution_duration << std::endl;
    }

    zmq::Registrar* master = 0;

    bool success = true;

    auto execution_manager = new ExecutionManager(master_endpoint,
                                                    graphml_path,
                                                    execution_duration,
                                                    exe,
                                                    experiment_id,
                                                    environment_manager_endpoint);
    master = new zmq::Registrar(execution_manager, master_endpoint);

    success = execution_manager->IsValid();

    if(success){
        //Use execution class to wait for interrupt
        exe->Start();
    }

    delete master;

    delete exe;
    return success ? 0 : 1;
}
