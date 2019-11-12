// REVIEW (Mitch): This file along with experimentmaster.cpp are unused.
#include <iostream>
#include <vector>
#include <string>
#include <signal.h>
#include <boost/program_options.hpp>

#include "deploymentmanager.h"

#include "cmakevars.h"
#include "execution.hpp"

#include "zmq/registrant.h"

std::string VERSION_NAME = "experiment_slave";

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
    std::string dll_path;
    std::string slave_endpoint;
    std::string environment_manager_endpoint;
    std::string experiment_id;
    bool live_logging = false;

    boost::program_options::options_description options("Node manager options");
    options.add_options()("library,l", boost::program_options::value<std::string>(&dll_path), "Library path");
    options.add_options()("slave,s", boost::program_options::value<std::string>(&slave_endpoint), "Slave endpoint, including port");
    options.add_options()("live-logging,L", boost::program_options::value<bool>(&live_logging), "Logging option");
    options.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint), "Environment manager endpoint.");
    options.add_options()("experiment-id,n", boost::program_options::value<std::string>(&experiment_id), "ID of experiment to attach slave to.");
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

    if(slave_endpoint.empty()){
        std::cerr << "Arg Error: experiment_slave requires a valid endpoint." << std::endl;
        valid_args = false;
    }
    if(dll_path.empty()){
        std::cerr << "Arg Error: experiment_slave requires a library path to execute." << std::endl;
        valid_args = false;
    }


    if(!valid_args){
        std::cout << options << std::endl;
        return 1;
    }


    std::cout << "* Slave:" << std::endl;
    std::cout << "** Endpoint: " << slave_endpoint << std::endl;
    std::cout << "** Library Path: " << dll_path << std::endl;

    zmq::Registrant* slave = 0;


    //Construct a Deployment Manager to handle the Deployment
    auto deployment_manager = new DeploymentManager(false, dll_path, exe, experiment_id, slave_endpoint, environment_manager_endpoint);
    slave = new zmq::Registrant(deployment_manager);

    //Use execution class to wait for interrupt
    exe->Start();

    delete slave;

    delete exe;
    return 0;
}
