#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>

#include "core/nodecontainer.h"

class DeploymentManager{
    public:
        DeploymentManager(std::string library_path);
        ~DeploymentManager();

        bool is_library_loaded();
        
        NodeContainer* get_deployment();

    private:
        std::string library_path_;
        void* library_handle_ = 0;

        NodeContainer* deployment_ = 0;

        std::function<NodeContainer* ()> create_deployment_ = 0;
        std::function<void (NodeContainer*)> destroy_deployment_ = 0;
};

#endif //DEPLOYMENTMANAGER_H