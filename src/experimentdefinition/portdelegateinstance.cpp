#include "portdelegateinstance.h"

namespace re::Representation {

PortDelegateInstance::PortDelegateInstance(const PortDelegateInstance::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    medea_aggregate_type_ = pb.medea_aggregate_type();

    for(const auto& connected_port_delegate_uuid : pb.connected_port_delegate_uuids()) {
        connected_port_delegate_uuids_.emplace_back(connected_port_delegate_uuid);
    }

    for(const auto& port_instance_uuid : pb.connected_port_instance_uuids()) {
        connected_port_instance_uuids_.emplace_back(port_instance_uuid);
    }
}
auto PortDelegateInstance::ToProto() const -> std::unique_ptr<PortDelegateInstance::PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());

    out->set_medea_aggregate_type(medea_aggregate_type_);

    for(const auto& delegate_uuid : connected_port_delegate_uuids_) {
        out->add_connected_port_delegate_uuids(delegate_uuid.to_string());
    }

    for(const auto& instance_uuid : connected_port_instance_uuids_) {
        out->add_connected_port_instance_uuids(instance_uuid.to_string());
    }
    return out;
}
} // namespace re::Representation
