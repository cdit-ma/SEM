#ifndef ENVIRONMENT_MANAGER_QPID_PORT_H
#define ENVIRONMENT_MANAGER_QPID_PORT_H

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
namespace qpid {

class Port : public ::EnvironmentManager::Port{
    public:
        Port(::EnvironmentManager::Component& parent, const NodeManager::Port& port);
        Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port);
        ~Port();
    protected:
        std::string GetBrokerAddress() const;
        std::string GetTopic() const;
        void FillPortPb(NodeManager::Port& port_pb);

    private:

        std::string topic_name_;
        std::string broker_address_;
};
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H