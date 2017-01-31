#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>

#include "nodecontainer.h"


class DeploymentManager{
    public:
        DeploymentManager(std::string library_path);
        ~DeploymentManager();

        void process_action(std::string node_name, std::string action);

        NodeContainer* get_deployment();

    private:
        std::string library_path_;
        std::mutex mutex_;
        

        NodeContainer* deployment_ = 0;
};

#endif //DEPLOYMENTMANAGER_H