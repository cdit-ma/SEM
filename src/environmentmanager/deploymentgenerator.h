#include "deploymentrule.h"
#include "environment.h"

#include <map>
#include <functional>
#include <list>

class DeploymentGenerator{
    public:
        DeploymentGenerator(Environment& environment);
        void PopulateDeployment(NodeManager::ControlMessage& message);
        void TerminateDeployment(NodeManager::ControlMessage& message);

        void AddDeploymentRule(std::unique_ptr<DeploymentRule> rule);

    private:

        DeploymentRule& GetDeploymentRule(DeploymentRule::MiddlewareType type);

        void AddExperiment(const NodeManager::ControlMessage& control_message);
        void AddNodeToExperiment(const std::string& experiment_id, const NodeManager::Node& node);
        void PopulateNode(const NodeManager::ControlMessage& control_message, NodeManager::Node& node);
        DeploymentRule::MiddlewareType MapMiddleware(NodeManager::Port::Middleware middleware);

        Environment& environment_;
        std::list<std::unique_ptr<DeploymentRule> > rules_;

};
