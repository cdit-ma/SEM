#include "componentinstance.h"
#include "middlewareportinstance.h"
#include "periodicportinstance.h"
#include "triggerinstance.h"
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
    definition_uuid_ = sem::types::Uuid(pb.definition_uuid());
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

ComponentInstance::ComponentInstance(GraphmlParser& parser,
                                     const std::string& medea_id,
                                     const sem::types::Uuid& definition_uuid) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}},
    medea_type_{parser.GetDataValue(medea_id, "type")},
    definition_uuid_{definition_uuid}
{
}
auto ComponentInstance::AddMiddlewarePortInstance(const MiddlewarePortInstance& port_instance)
    -> void
{
    middleware_port_instance_uuids_.emplace_back(port_instance.GetCoreData().GetUuid());
}
auto ComponentInstance::AddPeriodicPortInstance(const PeriodicPortInstance& port_instance) -> void
{
    periodic_port_instance_uuids_.emplace_back(port_instance.GetCoreData().GetUuid());
}
auto ComponentInstance::GetDefinitionUuid() const -> sem::types::Uuid
{
    return definition_uuid_;
}
auto ComponentInstance::GetMedeaType() const -> std::string
{
    return medea_type_;
}
auto ComponentInstance::AddTriggerInstance(const TriggerInstance& trigger_instance) -> void {
    trigger_instance_uuids_.emplace_back(trigger_instance.GetCoreData().GetUuid());
}
} // namespace re::Representation
