#include "deploymentgenerator.h"
#include "deploymentrule.h"
#include <iostream>
DeploymentGenerator::DeploymentGenerator(EnvironmentManager::Environment& environment) : environment_(environment){
}

NodeManager::ControlMessage* DeploymentGenerator::PopulateDeployment(NodeManager::ControlMessage& control_message){
    const auto& experiment_id = control_message.experiment_id();

    environment_.DeclusterExperiment(control_message);
    //Add experiment to environment
    AddExperiment(control_message);

    std::string master_ip_address;

    for(const auto& attribute : control_message.attributes()){
        if(attribute.info().name() == "master_ip_address"){
            master_ip_address = attribute.s(0);
            break;
        }
    }
    
    environment_.SetExperimentMasterIp(experiment_id, master_ip_address);
    environment_.ConfigureNodes(experiment_id);
    auto configured_message = environment_.GetProto(experiment_id);
    environment_.FinishConfigure(experiment_id);
    return configured_message;
}

void DeploymentGenerator::PopulateNode(const NodeManager::ControlMessage& control_message, NodeManager::Node& node){
    //TODO: Store node information in environment database?
    //Recurse down into subnodes
    for(auto& node : *node.mutable_nodes()){
        PopulateNode(control_message, node);
    }
}

void DeploymentGenerator::TerminateDeployment(NodeManager::ControlMessage& control_message){

}

void DeploymentGenerator::AddExperiment(const NodeManager::ControlMessage& control_message){
    std::string experiment_id(control_message.experiment_id());

    environment_.AddExternalPorts(experiment_id, control_message);
    for(const auto& node : control_message.nodes()){
        AddNodeToExperiment(experiment_id, node);
    }
}

void DeploymentGenerator::AddNodeToExperiment(const std::string& experiment_id, const NodeManager::Node& node){
    for(const auto& node : node.nodes()){
        AddNodeToExperiment(experiment_id, node);
    }
    environment_.AddNodeToExperiment(experiment_id, node);
}
