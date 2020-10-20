#include "componentdefinition.h"
#include "middlewareportdefinition.h"
#include "periodicportdefinition.h"
namespace re::Representation {
auto ComponentDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());

    for(const auto& ns : medea_namespaces_) {
        out->add_medea_namespaces(ns);
    }

    for(const auto& port_uuid : middleware_port_definition_uuids_) {
        out->add_middleware_port_definition_uuids(port_uuid.to_string());
    }

    for(const auto& periodic_port_uuid : periodic_port_definition_uuids_) {
        out->add_periodic_port_definition_uuids(periodic_port_uuid.to_string());
    }

    for(const auto& worker_uuid : worker_definition_uuids_) {
        out->add_worker_definition_uuids(worker_uuid.to_string());
    }

    for(const auto& attr_uuid : attribute_definition_uuids_) {
        out->add_attribute_definition_uuids(attr_uuid.to_string());
    }

    for(const auto& trigger_uuid : trigger_definition_uuids_) {
        out->add_trigger_definition_uuids(trigger_uuid.to_string());
    }
    return out;
}

ComponentDefinition::ComponentDefinition(const PbType& pb) : DefaultModelEntity{pb.core_data()}
{
    for(const auto& medea_namespace : pb.medea_namespaces()) {
        medea_namespaces_.emplace_back(medea_namespace);
    }

    for(const auto& port_definition_uuid : pb.middleware_port_definition_uuids()) {
        middleware_port_definition_uuids_.emplace_back(port_definition_uuid);
    }

    for(const auto& port_definition_uuid : pb.periodic_port_definition_uuids()) {
        periodic_port_definition_uuids_.emplace_back(port_definition_uuid);
    }

    for(const auto& worker_definition_uuid : pb.worker_definition_uuids()) {
        worker_definition_uuids_.emplace_back(worker_definition_uuid);
    }
    for(const auto& attr_uuid : pb.attribute_definition_uuids()) {
        attribute_definition_uuids_.emplace_back(attr_uuid);
    }
    for(const auto& trigger_definition_uuid : pb.trigger_definition_uuids()) {
        trigger_definition_uuids_.emplace_back(trigger_definition_uuid);
    }
}
ComponentDefinition::ComponentDefinition(GraphmlParser& parser, const std::string& medea_id) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}}
{
    // TODO: Populate namespaces.
}
auto ComponentDefinition::AddMiddlewarePortDefinition(
    const MiddlewarePortDefinition& port_definition) -> void
{
    middleware_port_definition_uuids_.emplace_back(port_definition.GetCoreData().GetUuid());
}

auto ComponentDefinition::AddPeriodicPortDefinition(const PeriodicPortDefinition& port_definition) -> void {
    periodic_port_definition_uuids_.emplace_back(port_definition.GetCoreData().GetUuid());
}
} // namespace re::Representation
