#include "deploymentgenerator.h"
#include "deploymentrule.h"
#include <iostream>
DeploymentGenerator::DeploymentGenerator(EnvironmentManager::Environment& environment) : environment_(environment){
}

NodeManager::ControlMessage* DeploymentGenerator::PopulateDeployment(NodeManager::ControlMessage& control_message){

    //Decluster operation mutates control message
    environment_.DeclusterExperiment(control_message);

    std::string experiment_id = control_message.experiment_id();

    //Add experiment to environment
    AddExperiment(control_message);

    std::string master_ip_address;
    for(int i = 0; i < control_message.attributes_size(); i++){
        auto attribute = control_message.attributes(i);
        if(attribute.info().name() == "master_ip_address"){
            master_ip_address = attribute.s(0);
        }
    }

    environment_.SetExperimentMasterIp(experiment_id, master_ip_address);

    environment_.ConfigureNodes(experiment_id);

    NodeManager::ControlMessage* configured_message = environment_.GetProto(experiment_id);
    environment_.FinishConfigure(experiment_id);

    return configured_message;
}

void DeploymentGenerator::PopulateNode(const NodeManager::ControlMessage& control_message, NodeManager::Node& node){

    //Store node information in environment database?

    //Recurse down into subnodes
    for(int i = 0; i < node.nodes_size(); i++){
        PopulateNode(control_message, *node.mutable_nodes(i));
    }
}

void DeploymentGenerator::TerminateDeployment(NodeManager::ControlMessage& control_message){

}

void DeploymentGenerator::AddExperiment(const NodeManager::ControlMessage& control_message){
    std::string experiment_id(control_message.experiment_id());

    environment_.AddExternalPorts(experiment_id, control_message);

    for(int i = 0; i < control_message.nodes_size(); i++){
        AddNodeToExperiment(experiment_id, control_message.nodes(i));
    }
}

void DeploymentGenerator::AddNodeToExperiment(const std::string& experiment_id, const NodeManager::Node& node){
    for(int i = 0; i < node.nodes_size(); i++){
        AddNodeToExperiment(experiment_id, node.nodes(i));
    }
    environment_.AddNodeToExperiment(experiment_id, node);
}
