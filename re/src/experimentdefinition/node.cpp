#include "node.h"

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
    DefaultModelEntity{pb.core_data()}, ip_address_{types::Ipv4::from_string(pb.ip_address())}
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
} // namespace re::Representation