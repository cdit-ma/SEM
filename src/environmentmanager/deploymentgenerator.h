#include "environment.h"

#include <map>
#include <functional>
#include <list>

class DeploymentGenerator{
    public:
        DeploymentGenerator(EnvironmentManager::Environment& environment);
        NodeManager::ControlMessage* PopulateDeployment(NodeManager::ControlMessage& message);
    private:
        EnvironmentManager::Environment& environment_;
};
