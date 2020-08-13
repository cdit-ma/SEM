#include "worker.h"
#include "attribute.h"
#include "component.h"
#include "helper.h"

using namespace EnvironmentManager;

Worker::Worker(Component& parent, const NodeManager::Worker& worker) : parent_(parent){
    id_ = worker.info().id();
    name_ = worker.info().name();
    type_ = worker.info().type();

    for(const auto& pair : worker.attributes()){
        const auto& id = pair.first;
        const auto& attr_pb = pair.second;
        auto attr = std::unique_ptr<EnvironmentManager::Attribute>(new EnvironmentManager::Attribute(attr_pb));
        attributes_.emplace(id, std::move(attr));
    }
}

void Worker::SetDirty(){
    parent_.SetDirty();
    dirty_ = true;
}

bool Worker::IsDirty(){
    return dirty_;
}

std::unique_ptr<NodeManager::Worker> Worker::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::Worker> worker;

    if(full_update || dirty_){
        worker = std::unique_ptr<NodeManager::Worker>(new NodeManager::Worker());
        worker->mutable_info()->set_name(name_);
        worker->mutable_info()->set_id(id_);
        worker->mutable_info()->set_type(type_);

        auto attr_map = worker->mutable_attributes();

        for(const auto& attribute : attributes_){
            auto attr_pb = attribute.second->GetProto(full_update);
            if(attr_pb){
                NodeManager::AddAllocatedAttribute(attr_map, std::move(attr_pb));
            }
        }

        if(dirty_){
            dirty_ = false;
        }
    }
    return worker;
}