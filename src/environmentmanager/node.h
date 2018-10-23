#ifndef ENVIRONMENT_MANAGER_NODE_H
#define ENVIRONMENT_MANAGER_NODE_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "uniquequeue.hpp"
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{

class Environment;
class Port;
class Experiment;
class Container;
class Logger;

class Node{
    public:
        Node(Environment& environment, Experiment& experiment, const NodeManager::Node& node);

        //Info getters
        std::string GetId() const;
        std::string GetName() const;
        std::string GetIp() const;

        void SetNodeManagerMaster();

        std::unique_ptr<NodeManager::HardwareId> GetHardwareId() const;
        std::vector<std::unique_ptr<NodeManager::ContainerId> > GetContainerIds() const;

        //Node, component, attribute and logger adders
        void AddContainer(const NodeManager::Container& component);

        //Update requirement management
        void SetDirty();
        bool IsDirty();

        Experiment& GetExperiment();

        //Deep getters
        bool HasContainer(const std::string& container_id) const;
        bool HasLogger(const std::string& logger_id) const;
        bool HasPort(const std::string& port_id) const;

        Container& GetContainer(const std::string& component_id) const;
        Logger& GetLogger(const std::string& logger_id) const;
        Port& GetPort(const std::string& port_id) const;

        int GetDeployedCount() const;
        int GetDeployedComponentCount() const;
        int GetContainerCount() const;
        int GetLoganServerCount() const;
        bool DeployedTo() const;

        std::vector<std::unique_ptr<NodeManager::Logger> > GetAllocatedLoganServers();

            //protobuf getters
        std::unique_ptr<NodeManager::Node> GetProto(const bool full_update);
        
    private:
        Environment& environment_;
        Experiment& experiment_;
        std::string id_;
        std::string name_;
        std::string ip_;

        //Container id -> Container
        std::unordered_map<std::string, std::unique_ptr<Container> > containers_;

        bool dirty_;
};
};

#endif //ENVIRONMENT_MANAGER_NODE_H