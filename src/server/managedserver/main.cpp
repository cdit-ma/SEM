#include <iostream>
#include <boost/program_options.hpp>
#include <util/execution.hpp>
#include "managedserver.h"
#include "cmakevars.h"


Execution execution;

void signal_handler(int sig)
{
    execution.Interrupt();
}

int main(int ac, char** av){
    //Connect the SIGINT/SIGTERM signals to our handler.
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

    std::string experiment_name;
    std::string environment_manager_endpoint;
    std::string ip_address;

    boost::program_options::options_description options(LONG_VERSION " Options");
    options.add_options()("address,a", boost::program_options::value<std::string>(&ip_address)->required(), "Node manager ip address.");
    options.add_options()("experiment-name,n", boost::program_options::value<std::string>(&experiment_name)->required(), "Name of experiment.");
    options.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_endpoint)->required(), "Environment manager fully qualified endpoint ie. (tcp://192.168.111.230:20000).");
    options.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(ac, av, options), vm);
        boost::program_options::notify(vm);
    }catch(const boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << options << std::endl;
        return 1;
    }

    try{
        ManagedServer server(execution, experiment_name, ip_address, environment_manager_endpoint);
        execution.Start();
    }catch(const ManagedServer::NotNeededException& ex){

    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}