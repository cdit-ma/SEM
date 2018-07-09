#include "deploymentgenerator.h"
#include "deploymentrule.h"
#include <iostream>
DeploymentGenerator::DeploymentGenerator(EnvironmentManager::Environment& environment) : environment_(environment){
}

NodeManager::ControlMessage* DeploymentGenerator::PopulateDeployment(NodeManager::ControlMessage& control_message){
    auto configured_message = environment_.PopulateDeployment(control_message);
    return configured_message;
}
