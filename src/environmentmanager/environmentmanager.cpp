#include <iostream>
#include <thread>
#include <chrono>
#include <boost/program_options.hpp>

#include "broadcaster.h"

int main(int argc, char **argv){



    std::string address("ADDRESS YO");
    Broadcaster* broadcaster = new Broadcaster(address);
    broadcaster->StartBroadcast();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}