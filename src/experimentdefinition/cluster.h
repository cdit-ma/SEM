#ifndef RE_CLUSTER_H
#define RE_CLUSTER_H

#include "modelentity.h"
#include "node.h"
#include <types/ipv4.hpp>
namespace re::Representation {

class Cluster : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::Cluster;
    explicit Cluster(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    types::Ipv4 jenkins_master_address_;
    std::vector<types::Uuid> node_uuids_;
};

} // namespace re::Representation
#endif // RE_CLUSTER_H
