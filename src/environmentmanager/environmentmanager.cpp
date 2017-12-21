#include <iostream>
#include <thread>
#include <chrono>
#include <boost/program_options.hpp>

#include "broadcaster.h"
#include "deploymentregister.h"

int main(int argc, char **argv){

    std::string address("tcp://*:22334");
    std::string message("tcp://192.168.111.230:22337");

    int registration_port = 22337;
    int hb_start_port = 12345;

    Broadcaster* broadcaster = new Broadcaster(address, message);
    broadcaster->StartBroadcast();

    DeploymentRegister* deployment_register = new DeploymentRegister("192.168.111.230", registration_port, hb_start_port);
    deployment_register->Start();


    //TODO: condition variable to control termination correctly.
    while(true){

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    return 0;
}