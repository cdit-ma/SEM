#include "container.h"
#include "cluster.h"
#include "componentassembly.h"
#include "componentinstance.h"
namespace re::Representation {
auto ContainerTypeToPb(Container::Type type) -> Container::PbType::ContainerType
{
    switch(type) {
        case Container::Type::Docker:
            return Container::PbType::Docker;
        case Container::Type::OsProcess:
            return Container::PbType::OsProcess;
    }
    throw std::invalid_argument("Invalid container type found in experiment_definition::container");
}
auto ContainerTypePbToType(Container::PbType::ContainerType type)
{
    switch(type) {
        case Container::PbType::OsProcess:
            return Container::Type::OsProcess;
        case Container::PbType::Docker:
            return Container::Type::Docker;
        case network::protocol::experimentdefinition::
            Container_ContainerType_Container_ContainerType_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::
            Container_ContainerType_Container_ContainerType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid container type found in "
                                "experiment_definition::container.");
}

auto Container::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_container_type(ContainerTypeToPb(container_type_));

    for(const auto& component_instance_uuid : component_instance_uuids_) {
        out->add_component_instance_uuids(component_instance_uuid.to_string());
    }

    for(const auto& component_assembly_uuid : component_assembly_uuids_) {
        out->add_component_assembly_uuids(component_assembly_uuid.to_string());
    }

    for(const auto& logger_definition_uuid : logger_definition_uuids_) {
        out->add_logger_definition_uuids(logger_definition_uuid.to_string());
    }
    return out;
}

Container::Container(const PbType& pb) : DefaultModelEntity{pb.core_data()}
{
    container_type_ = ContainerTypePbToType(pb.container_type());

    for(const auto& component_instance_uuid : pb.component_instance_uuids()) {
        component_instance_uuids_.emplace_back(component_instance_uuid);
    }

    for(const auto& component_assembly_uuid : pb.component_assembly_uuids()) {
        component_assembly_uuids_.emplace_back(component_assembly_uuid);
    }

    for(const auto& logger_definition_uuid : pb.logger_definition_uuids()) {
        logger_definition_uuids_.emplace_back(logger_definition_uuid);
    }
}

Container::Container(GraphmlParser& parser, const std::string& medea_id) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}}
{
    auto container_is_docker = (parser.GetDataValue(medea_id, "is_docker") == "true");

    if(container_is_docker) {
        container_type_ = Type::Docker;
    } else {
        container_type_ = Type::OsProcess;
    }
}

auto Container::AddDeployedLoggingClient(const LoggingClientDefinition& logging_client) -> void
{
    logger_definition_uuids_.push_back(logging_client.GetCoreData().GetUuid());
}

auto Container::AddDeployedComponentAssembly(const ComponentAssembly& component_assembly) -> void
{
    component_assembly_uuids_.push_back(component_assembly.GetCoreData().GetUuid());
}

auto Container::AddDeployedComponentInstance(const ComponentInstance& component_instance) -> void
{
    component_instance_uuids_.push_back(component_instance.GetCoreData().GetUuid());
}
auto Container::Duplicate() -> Container
{
    // Our duplicate should be same in every way except for its uuid
    CoreData new_core_data{sem::types::Uuid{}, GetCoreData().GetMedeaId(), GetCoreData().GetMedeaId()};
    auto out = Container(*this);
    out.SetCoreData(new_core_data);
    return out;
}
auto Container::GetType() const -> Container::Type
{
    return container_type_;
}
auto Container::GetDeployedComponents() const -> std::vector<sem::types::Uuid>
{
    return component_instance_uuids_;
}
} // namespace re::Representation
