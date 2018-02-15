#include "environmentrequester.h"
#include <iostream>

int main(){


    EnvironmentRequester* requester = new EnvironmentRequester("tcp://192.168.111.230:22334", "deployment01", "this is a deployment info");

    requester->Init();
    requester->Start();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //auto port = requester->GetComponentPort("component01", "info here yoo");

    auto port = requester->GetDeploymentMasterPort();
    std::cout << port << std::endl;
    auto logger_port = requester->GetModelLoggerPort();
    std::cout << logger_port << std::endl;
    // int counter = 0;
    // while(true){
    //     counter++;
    //     if(counter % 5 == 0){
    //         std::string comp_name = "component" + std::to_string(counter);
    //         std::cout << comp_name << std::endl;
    //         std::cout << requester->GetComponentPort(comp_name, "info yo") << std::endl;;
    //     }

    //     if(counter == 10){
    //         break;
    //     }
    // }

    // auto requester2 = new EnvironmentRequester("tcp://192.168.111.230:22334", "deployment02", "asdf");
    // requester2->Init();
    // requester2->Start();

    // std::cout << requester2->GetDeploymentMasterPort() << std::endl;
    // std::cout << requester2->GetModelLoggerPort() << std::endl;

    // std::cout << requester->GetComponentPort("componentid", "adsfasdf") << std::endl;
    // std::cout << requester2->GetComponentPort("componentid1", "adsfasdf") << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));
    requester->RemoveDeployment();
    std::cout << "removed ya" << std::endl;




    requester->End();
    //requester2->End();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    delete requester;
    //delete requester2;

    std::cout << "test yo" << std::endl;
    return 0;
}
