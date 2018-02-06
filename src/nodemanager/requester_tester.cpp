#include "environmentrequester.h"
#include <iostream>

int main(){


    EnvironmentRequester* requester = new EnvironmentRequester("tcp://192.168.111.230:22334", "deployment01", "this is a deployment info");

    requester->Init();
    requester->Start();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto port = requester->GetPort("component01", "info here yoo");
    std::cout << port << std::endl;
    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "test yo" << std::endl;
    return 0;
}