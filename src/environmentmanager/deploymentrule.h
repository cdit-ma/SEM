#ifndef DEPLOYMENTRULE_H
#define DEPLOYMENTRULE_H

#include "environment.h"

class DeploymentRule{
    public:
        enum class MiddlewareType{
            NONE = 0,
            ZMQ = 1,
            DDS = 2,
            AMQP = 3,
            TAO = 4
        };
        DeploymentRule(MiddlewareType type, Environment& environment) : environment_(environment){
            this->type = type;
        };

        virtual void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port) = 0;
        virtual void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port) = 0;

        MiddlewareType GetMiddlewareType() const { return type; };
    private:
        MiddlewareType type = MiddlewareType::NONE;
    protected:
            Environment& environment_;
};

#endif //DEPLOYMENTRULE_H
