#include "worker.h"
#include "attribute.h"
#include "component.h"
#include "helper.h"

#include <memory>

namespace re::EnvironmentManager {

Worker::Worker(Component& parent, const NodeManager::Worker& worker) : parent_(parent)
{
    id_ = worker.info().id();
    name_ = worker.info().name();
    type_ = worker.info().type();

    for(const auto& pair : worker.attributes()) {
        const auto& id = pair.first;
        const auto& attr_pb = pair.second;
        auto attr = std::make_unique<EnvironmentManager::Attribute>(attr_pb);
        attributes_.emplace(id, std::move(attr));
    }
}
Worker::Worker(const sem::types::Uuid& event_uuid,
               Component& component,
               const Representation::ExperimentDefinition& definition,
               const Representation::WorkerInstance& instance) :
    parent_(component)
{
    id_ = instance.GetCoreData().GetMedeaId() + "_" + event_uuid.to_string();
    name_ = instance.GetCoreData().GetMedeaName();
    auto worker_def = definition.GetWorkerDefinition(instance);
    type_ = worker_def.GetCppClassName();

    // TODO: Fill attributes
}
void Worker::SetDirty()
{
    parent_.SetDirty();
    dirty_ = true;
}

std::string Worker::GetId()
{
    return id_;
}

bool Worker::IsDirty()
{
    return dirty_;
}

std::unique_ptr<NodeManager::Worker> Worker::GetProto(const bool full_update)
{
    std::unique_ptr<NodeManager::Worker> worker;

    if(full_update || dirty_) {
        worker = std::make_unique<NodeManager::Worker>();
        worker->mutable_info()->set_name(name_);
        worker->mutable_info()->set_id(id_);
        worker->mutable_info()->set_type(type_);

        auto attr_map = worker->mutable_attributes();

        for(const auto& attribute : attributes_) {
            auto attr_pb = attribute.second->GetProto(full_update);
            if(attr_pb) {
                NodeManager::AddAllocatedAttribute(attr_map, std::move(attr_pb));
            }
        }

        if(dirty_) {
            dirty_ = false;
        }
    }
    return worker;
}

} // namespace re::EnvironmentManager