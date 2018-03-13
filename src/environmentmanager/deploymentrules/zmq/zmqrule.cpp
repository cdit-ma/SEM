#include "zmqrule.h"

Zmq::DeploymentRule::DeploymentRule() : ::DeploymentRule(::DeploymentRule::MiddlewareType::ZMQ){

}

void Zmq::DeploymentRule::ConfigureEventPort(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port){

}
void Zmq::DeploymentRule::TerminateEventPort(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port){

}