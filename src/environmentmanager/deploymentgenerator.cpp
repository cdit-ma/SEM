#include "deploymentgenerator.h"
#include "deploymentrule.h"

DeploymentGenerator::DeploymentGenerator(Environment& environment){
    environment_ = environment;
    zmq_deployment_rule_ = new Zmq::DeploymentRule(environment_);

    //Create middleware rule instances

    configure_rule_map_[DeploymentRule::MiddlewareType::ZMQ] = std::bind(&DeploymentRule::ConfigureEventPort, zmq_deployment_rule_, std::placeholders::_1);
    terminate_rule_map_[DeploymentRule::MiddlewareType::ZMQ] = std::bind(&DeploymentRule::TerminateEventPort, zmq_deployment_rule_, std::placeholders::_1);

    //Populate ControlMessage middleware enum -> DeploymentRule middleware enum map
    middleware_enum_map_[NodeManager::EventPort::ZMQ] = DeploymentRule::MiddlewareType::ZMQ;
    middleware_enum_map_[NodeManager::EventPort::RTI] = DeploymentRule::MiddlewareType::DDS;
    middleware_enum_map_[NodeManager::EventPort::OSPL] = DeploymentRule::MiddlewareType::DDS;
    middleware_enum_map_[NodeManager::EventPort::QPID] = DeploymentRule::MiddlewareType::AMQP;
    middleware_enum_map_[NodeManager::EventPort::TAO] = DeploymentRule::MiddlewareType::CORBA;

}

void DeploymentGenerator::PopulateDeployment(const NodeManager::ControlMessage& control_message){
    for(int i = 0; i < control_message.ndoes_size(); i++){
        NodeManager::Node* node = control_message.mutable_nodes(i);
        PopulateNode(control_message, node);
    }
}

void DeploymentGenerator::PopulateNode(const NodeManager::ControlMessage control_message&, NodeManager::Node& node){

    //Store node information in environment database?

    //Recurse down into subnodes
    for(int i = 0; i < node->nodes_size(); i++){
        PopulateNode(control_message, node->nodes(i));
    }

    //Hit bottom level sub node, or finished populating all subnodes. Fill this current node
    for(int i = 0; i < node->components_size(); i++){
        auto component = node->components(i);
        for(int j = 0; j < component.ports_size(); j++){
            NodeManager::EventPort* port = component->mutable_port(i);

            //Look up the correct rule function to populate the missing data in the port
            configure_rule_map_[middleware_enum_map_[port->middleware()]](control_message, port);
        }
    }
}

void DeploymentGenerator::TerminateDeployment(NodeManager::ControlMessage control_message){

}