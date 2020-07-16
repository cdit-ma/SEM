#ifndef ENVIRONMENT_MANAGER_WORKER_H
#define ENVIRONMENT_MANAGER_WORKER_H
#include <unordered_map>
#include "controlmessage.pb.h"

namespace EnvironmentManager{
class Component;
class Attribute;
class Worker{
    public:
        Worker(Component& parent, const NodeManager::Worker& worker);
        std::unique_ptr<NodeManager::Worker> GetProto(const bool full_update);

        void SetDirty();
        bool IsDirty();
    private:
        Component& parent_;
        std::string id_;
        std::string name_;
        std::string type_;
        bool dirty_;
        std::unordered_map<std::string, std::unique_ptr<Attribute> > attributes_;
};
}; // EnvironmentManager



#endif //ENVIRONMENT_MANAGER_WORKER_H