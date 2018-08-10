#include "worker.h"
#include "attribute.h"
#include "component.h"
#include <re_common/proto/controlmessage/helper.h>

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

NodeManager::Worker* Worker::GetUpdate(){
    auto worker = new NodeManager::Worker();
    worker->mutable_info()->set_name(name_);
    worker->mutable_info()->set_id(id_);
    worker->mutable_info()->set_type(type_);

    for(const auto& attribute : attributes_){
        auto attribute_proto = attribute.second->GetProto();

        if(attribute_proto){
            (*worker->mutable_attributes())[attribute.first] = *attribute_proto;
            delete attribute_proto;
        }
    }
    dirty_ = false;
    return worker;
}
NodeManager::Worker* Worker::GetProto(){
    auto worker = new NodeManager::Worker();

    worker->mutable_info()->set_name(name_);
    worker->mutable_info()->set_id(id_);
    worker->mutable_info()->set_type(type_);

    for(const auto& attribute : attributes_){
        NodeManager::AddAllocatedAttribute(worker->mutable_attributes(), attribute.second->GetProto());
    }
    return worker;
}