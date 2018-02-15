#include <environmentmanager/deploymentregister.h>
#include <nodemanager/environmentrequester.h>
#include "gtest/gtest.h"
#include <chrono>

TEST(Deployment, Registration1){
    //TODO: fix to use local host mode
    DeploymentRegister* deployment_register = new DeploymentRegister("localhost", "22337", 40000, 40100);
    deployment_register->Start();

    std::cout << "ehre" << std::endl;

    std::string deployment_id = "test_deployment_id01";
    EnvironmentRequester* requester = new EnvironmentRequester("", deployment_id, "");
    std::cout << "ehre" << std::endl;

    requester->Init("tcp://localhost:22337");
    std::this_thread::sleep_for(std::chrono::seconds(4));
    requester->Start();

    auto port = requester->GetDeploymentMasterPort();
    auto logger_port = requester->GetModelLoggerPort();



    requester->RemoveDeployment();

    delete requester;
    delete deployment_register;
}

TEST(Deployment, Registration2){

}