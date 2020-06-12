#include "cluster.h"

namespace re::Representation {

auto Cluster::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_jenkins_master_ip(jenkins_master_address_.to_string());

    for(const auto& node_uuid : node_uuids_) {
        out->add_node_uuids(node_uuid.to_string());
    }

    return out;
}
Cluster::Cluster(const Cluster::PbType& pb) :
    DefaultModelEntity{pb.core_data()},
    jenkins_master_address_{types::Ipv4::from_string(pb.jenkins_master_ip())}
{
    for(const auto& node_uuid : pb.node_uuids()) {
        node_uuids_.emplace_back(node_uuid);
    }
}
} // namespace re::Representation