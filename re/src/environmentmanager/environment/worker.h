#ifndef ENVIRONMENT_MANAGER_WORKER_H
#define ENVIRONMENT_MANAGER_WORKER_H
#include "experimentdefinition.h"
#include "controlmessage.pb.h"
#include "uuid.h"
#include <unordered_map>

namespace re::EnvironmentManager{
class Component;
class Attribute;
class Worker{
    public:
        Worker(Component& parent, const NodeManager::Worker& worker);
        Worker(const sem::types::Uuid& event_uuid,
               Component& component,
               const re::Representation::ExperimentDefinition& definition,
               const re::Representation::WorkerInstance& instance);
        std::unique_ptr<NodeManager::Worker> GetProto(const bool full_update);

        void SetDirty();
        bool IsDirty();
        std::string GetId();
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
