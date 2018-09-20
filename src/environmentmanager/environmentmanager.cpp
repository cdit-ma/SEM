#include <iostream>
#include <thread>
#include <chrono>
#include <exception>
#include <boost/program_options.hpp>
#include <signal.h>
#include <util/execution.hpp>

#include "cmakevars.h"

#include "deploymentregister.h"

std::string VERSION_NAME = "re_environment_manager";

Execution execution;

void signal_handler(int sig)
{
    execution.Interrupt();
}

int main(int argc, char **argv){

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    static const std::string default_registration_port = "20000";

    std::string ip_address;
    std::string registration_port;
    std::string qpid_address;
    std::string tao_naming_service_address;

    //Parse Args
    boost::program_options::options_description options("Environment Manager Options");
    options.add_options()("ip_address,a", boost::program_options::value<std::string>(&ip_address),
                            "Ip address of environment manager");
    options.add_options()("registration_port,r", boost::program_options::value<std::string>(&registration_port),
                            "Port number for deployment registration.");

    options.add_options()("qpid_address,q", boost::program_options::value<std::string>(&qpid_address),
                            "Endpoint of qpid broker.");
    
    options.add_options()("tao_naming_service_address,t", boost::program_options::value<std::string>(&tao_naming_service_address),
                            "Endpoint of tao naming service.");
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
        std::cerr << options << std::endl;
        return 0;
    }

    if(ip_address.empty()){
        std::cerr << "No ip address set, see --h for paramaters." << std::endl;
        std::cerr << options << std::endl;
        return 1;
    }

    //If any reqired options are empty, set to default.
    if(registration_port.empty()){
        registration_port = default_registration_port;
    }

    //Broker and naming service endpoints default to same ip as environment manager.
    if(qpid_address.empty()){
        qpid_address = ip_address + ":5672";
        std::cerr << "* No QPID broker address set, using default: " << qpid_address << std::endl;
    }
    if(tao_naming_service_address.empty()){
        std::cerr << "* No Tao naming service address set. This will cause models containing TAO ports to fail." << std::endl;
    }

    auto deployment_register = std::unique_ptr<DeploymentRegister>(new DeploymentRegister(execution, ip_address, registration_port, qpid_address, tao_naming_service_address));
    deployment_register->Start();

    std::cout << "-------[" + VERSION_NAME +" v" + RE_VERSION + "]-------" << std::endl;
    std::cout << "* Endpoint: " << "tcp://" << ip_address << ":" << registration_port << std::endl;
    std::cout << "* Qpid Broker Address: " << qpid_address << std::endl;
    std::cout << "* Tao Naming Service Endpoint: " << "corbaloc:iiop:" << tao_naming_service_address << std::endl << std::endl;
    std::cout << "------------------[Running]------------------" << std::endl << std::endl;
    execution.Start();
    return 0;
}
