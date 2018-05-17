#include <iostream>
#include <thread>
#include <chrono>
#include <exception>
#include <boost/program_options.hpp>

#include "broadcaster.h"
#include "deploymentregister.h"

int main(int argc, char **argv){
    static const std::string default_bcast_port = "22334";
    static const std::string default_registration_port = "22335";

    std::string ip_address;
    std::string registration_port;
    std::string bcast_port;

    //Parse Args
    boost::program_options::options_description options("Environment Manager Options");
    options.add_options()("ip_address,a", boost::program_options::value<std::string>(&ip_address),
                            "Ip address of environment manager");
    options.add_options()("broadcast_port,b", boost::program_options::value<std::string>(&bcast_port),
                            "Port number for env manager address broadcast.");
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
    
    //Set defaults if not set in args
    if(bcast_port.empty()){
        bcast_port = default_bcast_port;
    }
    if(registration_port.empty()){
        registration_port = default_registration_port;
    }

    std::string bcast_address("tcp://" + ip_address + ":" + bcast_port);
    std::string bcast_message("tcp://" + ip_address + ":" + registration_port);

    //auto broadcaster = std::unique_ptr<Broadcaster>(new Broadcaster(bcast_address, bcast_message));
    //broadcaster->StartBroadcast();

    auto deployment_register = std::unique_ptr<DeploymentRegister>(new DeploymentRegister(ip_address, registration_port));
    deployment_register->Start();

    //TODO: condition variable to control termination correctly.
    while(true){

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
