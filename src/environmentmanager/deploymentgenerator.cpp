#include "deploymentgenerator.h"
#include "deploymentrule.h"
#include <iostream>
DeploymentGenerator::DeploymentGenerator(Environment& environment) : environment_(environment){
}

void DeploymentGenerator::PopulateDeployment(NodeManager::ControlMessage& control_message){
    //Add experiment to environment
    environment_.DeclusterExperiment(control_message);

    AddExperiment(control_message);

    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;

    std::string master_ip_address;
    for(int i = 0; i < control_message.attributes_size(); i++){
        auto attribute = control_message.attributes(i);
        if(attribute.info().name() == "master_ip_address"){
            master_ip_address = attribute.s(0);
        }
    }
    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;


    environment_.SetExperimentMasterIp(control_message.experiment_id(), master_ip_address);
    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;

    for(int i = 0; i < control_message.nodes_size(); i++){
        NodeManager::Node* node = control_message.mutable_nodes(i);
        PopulateNode(control_message, *node);
    }
    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;

    auto master_publisher_port_attribute = control_message.add_attributes();
    auto master_publisher_port_attribute_info = master_publisher_port_attribute->mutable_info();
    master_publisher_port_attribute_info->set_name("master_publisher_endpoint");
    master_publisher_port_attribute->set_kind(NodeManager::Attribute::STRING);
    master_publisher_port_attribute->add_s(environment_.GetMasterPublisherAddress(control_message.experiment_id()));
    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;


    auto master_registration_port_attribute = control_message.add_attributes();
    auto master_registration_port_attribute_info = master_registration_port_attribute->mutable_info();
    master_registration_port_attribute_info->set_name("master_registration_endpoint");
    master_registration_port_attribute->set_kind(NodeManager::Attribute::STRING);
    master_registration_port_attribute->add_s(environment_.GetMasterRegistrationAddress(control_message.experiment_id()));

    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;
    environment_.StoreControlMessage(control_message);
    std::cout <<  "SAFDADSFFDSFDSFDSFDSDSFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" << std::endl;

}

void DeploymentGenerator::PopulateNode(const NodeManager::ControlMessage& control_message, NodeManager::Node& node){

    //Store node information in environment database?

    //Recurse down into subnodes
    for(int i = 0; i < node.nodes_size(); i++){
        PopulateNode(control_message, *node.mutable_nodes(i));
    }

    //Hit bottom level sub node, or finished populating all subnodes. Fill this current node
    for(auto& component : *node.mutable_components()){
        for(auto& port : *component.mutable_ports()){
            if(port.kind() != NodeManager::Port::PERIODIC){
                auto& rule = GetDeploymentRule(MapMiddleware(port.middleware()));
                try{
                    rule.ConfigureEventPort(control_message, port);
                }
                catch(std::exception& ex){
                    std::cerr << "DeploymentGenerator::PopulateNode" << ex.what() << std::endl;
                }
            }
        }
    }
    //Populate management ports and update environment's understanding of this node
    environment_.ConfigureNode(control_message.experiment_id(), node);
}

void DeploymentGenerator::TerminateDeployment(NodeManager::ControlMessage& control_message){

}

DeploymentRule::MiddlewareType DeploymentGenerator::MapMiddleware(NodeManager::Middleware middleware){
    switch(middleware){
        case NodeManager::ZMQ:       return DeploymentRule::MiddlewareType::ZMQ;
        case NodeManager::RTI:       return DeploymentRule::MiddlewareType::DDS;
        case NodeManager::OSPL:      return DeploymentRule::MiddlewareType::DDS;
        case NodeManager::QPID:      return DeploymentRule::MiddlewareType::AMQP;
        case NodeManager::TAO:       return DeploymentRule::MiddlewareType::TAO;
        default:                     return DeploymentRule::MiddlewareType::NONE;
    }
}

 void DeploymentGenerator::AddDeploymentRule(std::unique_ptr<DeploymentRule> rule){
     rules_.emplace_back(std::move(rule));
 }

DeploymentRule& DeploymentGenerator::GetDeploymentRule(DeploymentRule::MiddlewareType type){
    for(auto& rule : rules_){
        if(rule->GetMiddlewareType() == type){
            return *rule;
        }
    }
    throw std::invalid_argument("No middleware rule supplied." + std::to_string((int)type));
}

void DeploymentGenerator::AddExperiment(const NodeManager::ControlMessage& control_message){
    std::string experiment_id(control_message.experiment_id());

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
