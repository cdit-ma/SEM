#include <iostream>
#include <thread>
#include <chrono>
#include <exception>
#include <boost/program_options.hpp>
#include <signal.h>
#include <re_common/util/execution.hpp>

#include "cmakevars.h"

#include "deploymentregister.h"

std::string VERSION_NAME = "re_environment_manager";

Execution* exe = 0;

void signal_handler(int sig)
{
    exe->Interrupt();
}

int main(int argc, char **argv){

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    exe = new Execution();

    static const std::string default_registration_port = "20000";

    std::string ip_address;
    std::string registration_port;

    //Parse Args
    boost::program_options::options_description options("Environment Manager Options");
    options.add_options()("ip_address,a", boost::program_options::value<std::string>(&ip_address),
                            "Ip address of environment manager");
    options.add_options()("registration_port,r", boost::program_options::value<std::string>(&registration_port),
                            "Port number for deployment registration.");
    options.add_options()("help,h", "Display help");

    boost::program_options::variables_map vm;

    try{
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);
        boost::program_options::notify(vm);
    }
    catch(...){
        std::cout << options << std::endl;
        return 1;
    }

    if(vm.count("help")){
        std::cout << options << std::endl;
        return 0;
    }

    if(registration_port.empty()){
        registration_port = default_registration_port;
    }

    auto deployment_register = std::unique_ptr<DeploymentRegister>(new DeploymentRegister(*exe, ip_address, registration_port));
    deployment_register->Start();

    std::cout << "-------[" + VERSION_NAME +" v" + RE_VERSION + "]-------" << std::endl;
    std::cout << "* Endpoint: " << "tcp://" << ip_address << ":" << registration_port << std::endl << std::endl;
    std::cout << "------------------[Running]------------------" << std::endl << std::endl;

    exe->Start();

    delete exe;
    std::cout << std::endl;

    return 0;
}
