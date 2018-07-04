#include "environment.h"

#include <map>
#include <functional>
#include <list>

class DeploymentGenerator{
    public:
        DeploymentGenerator(EnvironmentManager::Environment& environment);
        NodeManager::ControlMessage* PopulateDeployment(NodeManager::ControlMessage& message);
        void TerminateDeployment(NodeManager::ControlMessage& message);


    private:

        void AddExperiment(const NodeManager::ControlMessage& control_message);
        void AddNodeToExperiment(const std::string& experiment_id, const NodeManager::Node& node);
        void PopulateNode(const NodeManager::ControlMessage& control_message, NodeManager::Node& node);

        EnvironmentManager::Environment& environment_;

};
