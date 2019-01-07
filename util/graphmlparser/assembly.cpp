#include <sstream>
#include "assembly.h"

ComponentReplication::ComponentReplication(const std::string& component_instance_id, Assembly *parent) :
    component_instance_id_(component_instance_id),
    parent_(parent)
{
}

std::list<std::string> ComponentReplication::GetReplicationLocation() const {
    std::list<std::string> locations;

    auto current = parent_;
    while(current && current->GetParent()) {
        locations.push_front(current->GetName());
        current = current->GetParent();
    }
    return locations;
}

std::list<int> ComponentReplication::GetReplicationIndices() const {
    std::list<int> replications;

    auto current = parent_;
    //Ignore the Assemblies fam
    while(current && current->GetParent()){
        replications.push_front(current->GetReplicationIndex());
        current = current->GetParent();
    }
    return replications;
}

std::string ComponentReplication::GetUniqueId() const {
    std::stringstream ss;
    ss << component_instance_id_ << GetUniqueIdSuffix();
    return ss.str();
}

std::string ComponentReplication::GetUniqueIdSuffix() const {
    const auto& indices = GetReplicationIndices();

    return Assembly::GetUniqueSuffix({indices.begin(), indices.end()});
}

Assembly::Assembly(const std::string& name, const std::string& assembly_id, Assembly* parent) :
    name_(name),
    assembly_id_(assembly_id),
    parent_(parent)
{


}

Assembly::Assembly(Assembly& assembly) :
    Assembly(assembly.GetName(), assembly.GetAssemblyId(), assembly.parent_)
{
    replication_count_ = assembly.replication_count_;
}

Assembly* Assembly::GetParent() const {
    return parent_;
}

std::string Assembly::GetName() const {
    return name_;
}

unsigned int Assembly::GetReplicationIndex() const {
    return replication_index_;
}

std::string Assembly::GetUniqueSuffix(const std::vector<int> &indices) {
    std::stringstream ss;
    for(const auto& id : indices){
        ss << "_" << id;
    }
    return ss.str();
}

void Assembly::SetReplicationCount(unsigned int replication_count) {
    replication_count_ = replication_count;
}

std::string Assembly::GetAssemblyId() const {
    return assembly_id_;
}

unsigned int Assembly::GetReplicationCount() const {
    return replication_count_;
}

void Assembly::SetReplicationIndex(unsigned int replication_index) {
    replication_index_ = replication_index;
}

void Assembly::AddChild(std::unique_ptr<Assembly> child) {
    children_.push_back(std::move(child));
}

