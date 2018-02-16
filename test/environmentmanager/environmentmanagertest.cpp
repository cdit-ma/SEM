#include <environmentmanager/deploymentregister.h>
#include <nodemanager/environmentrequester.h>
#include "gtest/gtest.h"
#include <chrono>

TEST(Deployment, Registration1){
    //TODO: fix to use local host mode

    //192.168.111.230
    DeploymentRegister* deployment_register = new DeploymentRegister("192.168.111.230", "22337", 40000, 40100);
    deployment_register->Start();

    std::string deployment_id = "test_deployment_id01";
    EnvironmentRequester* requester = new EnvironmentRequester("", deployment_id, "");

    requester->Init("tcp://192.168.111.230:22337");
    
    requester->Start();

    auto port = requester->GetDeploymentMasterPort();
    
    auto logger_port = requester->GetModelLoggerPort();

    EXPECT_EQ(port, 40001);
    EXPECT_EQ(logger_port, 40002);

    requester->RemoveDeployment();

    delete requester;
    delete deployment_register;

    EXPECT_TRUE(true);
}

TEST(Deployment, Registration2){
    
}