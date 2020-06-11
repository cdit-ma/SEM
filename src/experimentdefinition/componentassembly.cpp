#include "componentassembly.h"
namespace re::Representation {

auto ComponentAssembly::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());

    out->set_replication_count(replication_count_);
    for(const auto& component_assembly_uuid : component_assembly_uuids_) {
        out->add_component_assembly_uuids(component_assembly_uuid.to_string());
    }
    for(const auto& component_uuid : component_instance_uuids_) {
        out->add_component_instance_uuids(component_uuid.to_string());
    }

    for(const auto& delegate_uuid : port_delegate_uuids_) {
        out->add_port_delegate_uuids(delegate_uuid.to_string());
    }
    return out;
}

ComponentAssembly::ComponentAssembly(const ComponentAssembly::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    replication_count_ = pb.replication_count();

    for(const auto& component_assembly_uuid : pb.component_assembly_uuids()) {
        component_assembly_uuids_.emplace_back(component_assembly_uuid);
    }

    for(const auto& component_uuid : pb.component_instance_uuids()) {
        component_instance_uuids_.emplace_back(component_uuid);
    }

    for(const auto& delegate_uuid : pb.port_delegate_uuids()) {
        port_delegate_uuids_.emplace_back(delegate_uuid);
    }
}
} // namespace re::Representation
