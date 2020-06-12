#include "componentinstance.h"
namespace re::Representation {

auto ComponentInstance::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_definition_uuid(definition_uuid_.to_string());
    out->set_medea_type(medea_type_);

    for(const auto& middleware_port_instance_uuid : middleware_port_instance_uuids_) {
        out->add_middleware_port_instance_uuids(middleware_port_instance_uuid.to_string());
    }

    for(const auto& periodic_port_instance_uuid : periodic_port_instance_uuids_) {
        out->add_periodic_port_instance_uuids(periodic_port_instance_uuid.to_string());
    }

    for(const auto& worker_instance_uuid : worker_instance_uuids_) {
        out->add_worker_instance_uuids(worker_instance_uuid.to_string());
    }

    for(const auto& attribute_instance_uuid : attribute_instance_uuids_) {
        out->add_attribute_instance_uuids(attribute_instance_uuid.to_string());
    }

    for(const auto& trigger_instance_uuid : trigger_instance_uuids_) {
        out->add_trigger_instance_uuids(trigger_instance_uuid.to_string());
    }
    return out;
}
ComponentInstance::ComponentInstance(const ComponentInstance::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    definition_uuid_ = types::Uuid(pb.definition_uuid());
    medea_type_ = pb.medea_type();

    for(const auto& middleware_port_instance_uuid : pb.middleware_port_instance_uuids()) {
        middleware_port_instance_uuids_.emplace_back(middleware_port_instance_uuid);
    }

    for(const auto& periodic_port_instance_uuid : pb.periodic_port_instance_uuids()) {
        periodic_port_instance_uuids_.emplace_back(periodic_port_instance_uuid);
    }

    for(const auto& worker_instance_uuid : pb.worker_instance_uuids()) {
        worker_instance_uuids_.emplace_back(worker_instance_uuid);
    }

    for(const auto& attribute_instance_uuid : pb.attribute_instance_uuids()) {
        attribute_instance_uuids_.emplace_back(attribute_instance_uuid);
    }

    for(const auto& trigger_instance_uuid : pb.trigger_instance_uuids()) {
        trigger_instance_uuids_.emplace_back(trigger_instance_uuid);
    }
}
} // namespace re::Representation
