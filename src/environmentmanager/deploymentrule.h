#ifndef DEPLOYMENTRULE_H
#define DEPLOYMENTRULE_H

#include "controlmessage.pb.h"

class DeploymentRule{
    public:
        enum class MiddlewareType{
            NONE,
            ZMQ,
            DDS,
            AMQP,
            CORBA
        };
        DeploymentRule(MiddlewareType type){
            this->type = type;
        };

        virtual void ConfigureEventPort(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port) = 0;
        virtual void TerminateEventPort(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port) = 0;

        MiddlewareType GetMiddlewareType() const { return type; };
    private:
        MiddlewareType type = MiddlewareType::NONE;
};

#endif //DEPLOYMENTRULE_H
