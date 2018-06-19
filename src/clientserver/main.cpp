#include <signal.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <re_common/util/execution.hpp>
#include "clientserver.h"
#include <zmq.hpp>

#include "cmakevars.h"

Execution* execution = 0;



int main(int ac, char** av){

    execution = new Execution();

    std::string experiment_id;
    std::string environment_manager_address;
    std::string address;

    boost::program_options::options_description desc(LONG_VERSION " Options");
    desc.add_options()("experiment-id,n", boost::program_options::value<std::string>(&experiment_id), "Experiment ID to log");
    desc.add_options()("environment-manager,e", boost::program_options::value<std::string>(&environment_manager_address), "Address of environment manager to connect to.");
    desc.add_options()("address,a", boost::program_options::value<std::string>(&address), "Address of this node.");
    desc.add_options()("help,h", "Display help");

    //Construct a variable_map
    boost::program_options::variables_map vm;

    try{
        //Parse Argument variables
        boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);
        boost::program_options::notify(vm);
    }catch(const boost::program_options::error& e) {
        std::cerr << "Arg Error: " << e.what() << std::endl << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }


    if(!(vm.count("experiment-id") && vm.count("environment-manager") && vm.count("address"))){
        std::cout << desc << std::endl;
        return 1;
    }

    try{
        ClientServer client_server(*execution, address, experiment_id, environment_manager_address);
        std::cout << "-------[ " LONG_VERSION " ]-------" << std::endl;
        execution->Start();
    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;

}