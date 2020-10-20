#include "componentassembly.h"
#include "componentinstance.h"
#include "portdelegateinstance.h"
namespace re::Representation {

auto ComponentAssembly::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());

    // TODO: Handle other variant case
    out->set_replication_count(std::get<uint64_t>(replication_));

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
    replication_ = pb.replication_count();
    // TODO: Handle other replication variant case

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

ComponentAssembly::ComponentAssembly(GraphmlParser& parser, const std::string& medea_id) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}}
{
    // Note the difference between replication count and replication value.
    auto replication_value = parser.GetDataValue(medea_id, "replicate_value");
    if(replication_value.empty()) {
        replication_ = (uint64_t)std::stoi(parser.GetDataValue(medea_id, "replicate_count"));
    } else {
        try {
            replication_ = (uint64_t)std::stoi(replication_value);
        } catch(const std::invalid_argument& ex) {
            // TODO: Handle replication value being non-integer. This is the case where replication
            //  count is driven by a deployment attribute
        }
    }
}

void ComponentAssembly::AddComponentInstance(const ComponentInstance& component_instance)
{
    component_instance_uuids_.emplace_back(component_instance.GetCoreData().GetUuid());
}

void ComponentAssembly::AddComponentAssembly(const ComponentAssembly& component_assembly)
{
    component_assembly_uuids_.emplace_back(component_assembly.GetCoreData().GetUuid());
}

void ComponentAssembly::AddPortDelegateInstance(const PortDelegateInstance& port_delegate_instance)
{
    port_delegate_uuids_.emplace_back(port_delegate_instance.GetCoreData().GetUuid());
}

} // namespace re::Representation
