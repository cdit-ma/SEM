#ifndef ENVIRONMENT_MANAGER_DDS_PORT_H
#define ENVIRONMENT_MANAGER_DDS_PORT_H

#include <mutex>
#include <memory>
#include "controlmessage.pb.h"

#include "port.h"

namespace re::EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Component;    
class Attribute;
namespace dds {

class Port : public EnvironmentManager::Port{
    public:
        Port(Component& parent, const NodeManager::Port& port);
        Port(Experiment& parent, const NodeManager::ExternalPort& port);
    protected:
        std::string GetTopic() const;
        int GetDomainId() const;

        void FillPortPb(NodeManager::Port& port_pb) final;

    private:
        std::string topic_name_;
        int domain_id_ = 0;
};
};
};

#endif //ENVIRONMENT_MANAGER_DDS_PORT_H