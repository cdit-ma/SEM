#ifndef ENVIRONMENT_MANAGER_ZMQ_PORT_H
#define ENVIRONMENT_MANAGER_ZMQ_PORT_H

#include <mutex>
#include <memory>
#include <re_common/proto/controlmessage/controlmessage.pb.h>

#include "port.h"

namespace EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Component;
class Attribute;
namespace zmq {

class Port : public ::EnvironmentManager::Port{
    public:
        Port(Component& parent, const NodeManager::Port& port);
        Port(Experiment& parent, const NodeManager::ExternalPort& port);
        ~Port();
    protected:
        std::string GetProducerPort() const;
        std::string GetProducerEndpoint() const;
        void FillPortPb(NodeManager::Port& port_pb);

    private:
        std::string ip_;
        std::string producer_port_;
};
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H