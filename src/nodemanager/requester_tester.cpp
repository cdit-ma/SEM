#include "environmentrequester.h"
#include <iostream>

int main(){


    EnvironmentRequester* requester = new EnvironmentRequester("tcp://192.168.111.230:22334", "deployment01", "this is a deployment info");

    requester->Init();
    requester->Start();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto port = requester->GetPort("component01", "info here yoo");
    std::cout << port << std::endl;
    int counter = 0;
    while(true){
        counter++;
        if(counter % 5 == 0){
            std::string comp_name = "component" + std::to_string(counter);
            std::cout << comp_name << std::endl;
            std::cout << requester->GetPort(comp_name, "info yo") << std::endl;;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if(counter == 10){
            break;
        }
    }

    requester->End();

    std::cout << "test yo" << std::endl;
    return 0;
}