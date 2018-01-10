#include <iostream>
#include <thread>
#include <chrono>
#include <boost/program_options.hpp>

#include "broadcaster.h"
#include "deploymentregister.h"

int main(int argc, char **argv){

    std::string ip_address("192.168.111.230");
    std::string registration_port("22337");

    std::string bcast_address("tcp://*:22334");
    std::string bcast_message("tcp://" + ip_address + ":" + registration_port);

    Broadcaster* broadcaster = new Broadcaster(bcast_address, bcast_message);
    broadcaster->StartBroadcast();

    DeploymentRegister* deployment_register = new DeploymentRegister(ip_address, registration_port);
    deployment_register->Start();


    //TODO: condition variable to control termination correctly.
    while(true){

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}
