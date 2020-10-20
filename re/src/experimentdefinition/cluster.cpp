#include "cluster.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include "container.h"

namespace re::Representation {

auto Cluster::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_jenkins_url(jenkins_url_);

    for(const auto& node_uuid : node_uuids_) {
        out->add_node_uuids(node_uuid.to_string());
    }

    for(const auto& instance_uuid : deployed_component_instance_uuids_) {
        out->add_deployed_component_instance_uuids(instance_uuid.to_string());
    }

    for(const auto& deployed_container_uuid : deployed_container_uuids_) {
        out->add_deployed_container_uuids(deployed_container_uuid.to_string());
    }

    for(const auto& logging_server_uuid : deployed_logging_server_uuids_) {
        out->add_deployed_logging_server_uuids(logging_server_uuid.to_string());
    }

    for(const auto& client_uuid : deployed_logging_client_uuids_) {
        out->add_deployed_logging_client_uuids(client_uuid.to_string());
    }

    for(const auto& component_assembly_uuid : deployed_component_assembly_uuids_) {
        out->add_deployed_component_assembly_uuids(component_assembly_uuid.to_string());
    }

    return out;
}

Cluster::Cluster(const Cluster::PbType& pb) :
    DefaultModelEntity{pb.core_data()}, jenkins_url_{pb.jenkins_url()}
{
    for(const auto& node_uuid : pb.node_uuids()) {
        node_uuids_.emplace_back(node_uuid);
    }

    for(const auto& component_instance_uuid : pb.deployed_component_instance_uuids()) {
        deployed_component_instance_uuids_.emplace_back(component_instance_uuid);
    }
    for(const auto& uuid : pb.deployed_container_uuids()) {
        deployed_container_uuids_.emplace_back(uuid);
    }
    for(const auto& uuid : pb.deployed_logging_client_uuids()) {
        deployed_logging_client_uuids_.emplace_back(uuid);
    }
    for(const auto& uuid : pb.deployed_logging_server_uuids()) {
        deployed_logging_server_uuids_.emplace_back(uuid);
    }
    for(const auto& uuid : pb.deployed_component_assembly_uuids()) {
        deployed_component_assembly_uuids_.emplace_back(uuid);
    }
}

Cluster::Cluster(GraphmlParser& graphml_parser, std::string medea_id) :
    DefaultModelEntity{{sem::types::Uuid{graphml_parser.GetDataValue(medea_id, "uuid")}, medea_id,
                        graphml_parser.GetDataValue(medea_id, "label")}},
    jenkins_url_{graphml_parser.GetDataValue(medea_id, "url")}
{
}

void Cluster::AddDeployedContainer(const Container& container)
{
    deployed_container_uuids_.push_back(container.GetCoreData().GetUuid());
}

void Cluster::AddDeployedLoggingClient(const LoggingClientDefinition& logging_client_definition)
{
    deployed_logging_client_uuids_.push_back(logging_client_definition.GetCoreData().GetUuid());
}

void Cluster::AddNode(const Node& node)
{
    node_uuids_.push_back(node.GetCoreData().GetUuid());
}

void Cluster::AddDeployedComponentAssembly(const ComponentAssembly& component_assembly)
{
    deployed_component_assembly_uuids_.emplace_back(component_assembly.GetCoreData().GetUuid());
}

void Cluster::AddDeployedComponentInstance(const ComponentInstance& component_instance)
{
    deployed_component_instance_uuids_.emplace_back(component_instance.GetCoreData().GetUuid());
}
auto Cluster::HasDeployedContainer(const Container& container) const -> bool
{
    return std::find(deployed_container_uuids_.begin(), deployed_container_uuids_.end(),
                     container.GetCoreData().GetUuid())
           != deployed_container_uuids_.end();
}

} // namespace re::Representation