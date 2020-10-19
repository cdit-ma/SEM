#include "node.h"
#include "componentassembly.h"
#include "componentinstance.h"
#include "container.h"
#include "loggingclientdefinition.h"
#include "loggingserverdefinition.h"

namespace re::Representation {

auto Node::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_ip_address(ip_address_.to_string());

    for(const auto& deployed_container_uuid : deployed_container_uuids_) {
        out->add_deployed_container_uuids(deployed_container_uuid.to_string());
    }

    for(const auto& component_instance_uuid : deployed_component_instance_uuids_) {
        out->add_deployed_component_instance_uuids(component_instance_uuid.to_string());
    }

    for(const auto& deployed_logging_client_uuid : deployed_logging_client_uuids_) {
        out->add_deployed_logging_client_uuids(deployed_logging_client_uuid.to_string());
    }

    for(const auto& logging_server_uuid : deployed_logging_server_uuids_) {
        out->add_deployed_logging_server_uuids(logging_server_uuid.to_string());
    }

    return out;
}
Node::Node(const PbType& pb) :
    DefaultModelEntity{pb.core_data()}, ip_address_{types::Ipv4(pb.ip_address())}
{
    for(const auto& deployed_container_uuid : pb.deployed_container_uuids()) {
        deployed_container_uuids_.emplace_back(deployed_container_uuid);
    }

    for(const auto& deployed_component_instance_uuid : pb.deployed_component_instance_uuids()) {
        deployed_component_instance_uuids_.emplace_back(deployed_component_instance_uuid);
    }

    for(const auto& deployed_logging_client_uuid : pb.deployed_logging_client_uuids()) {
        deployed_logging_client_uuids_.emplace_back(deployed_logging_client_uuid);
    }

    for(const auto& deployed_logging_server_uuid : pb.deployed_logging_server_uuids()) {
        deployed_logging_server_uuids_.emplace_back(deployed_logging_server_uuid);
    }
}
Node::Node(GraphmlParser& parser, const std::string& medea_id) :
    DefaultModelEntity{{types::Uuid{parser.GetDataValue(medea_id, "uuid")}, medea_id,
                        parser.GetDataValue(medea_id, "label")}},
    ip_address_(types::Ipv4(parser.GetDataValue(medea_id, "ip_address")))
{
}
auto Node::AddDeployedContainer(const Container& container) -> void
{
    deployed_container_uuids_.push_back(container.GetCoreData().GetUuid());
}
auto Node::AddDeployedLoggingClient(const LoggingClientDefinition& logging_client_definition)
    -> void
{
    deployed_logging_client_uuids_.push_back(logging_client_definition.GetCoreData().GetUuid());
}
auto Node::AddDeployedLoggingServer(const LoggingServerDefinition& logging_server_definition)
    -> void
{
    deployed_logging_server_uuids_.push_back(logging_server_definition.GetCoreData().GetUuid());
}
auto Node::AddDeployedComponentInstance(const ComponentInstance& component_instance) -> void
{
    deployed_component_instance_uuids_.push_back(component_instance.GetCoreData().GetUuid());
}
auto Node::AddDeployedComponentAssembly(const ComponentAssembly& component_assembly) -> void
{
    deployed_component_assembly_uuids.push_back(component_assembly.GetCoreData().GetUuid());
}
auto Node::HasDeployedContainer(const Container& container) const -> bool
{
    return std::find(deployed_container_uuids_.begin(), deployed_container_uuids_.end(),
                     container.GetCoreData().GetUuid())
           != deployed_container_uuids_.end();
}
auto Node::GetIpAddress() const -> types::Ipv4
{
    return ip_address_;
}

} // namespace re::Representation