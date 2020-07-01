#ifndef RE_NODE_H
#define RE_NODE_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "ipv4.hpp"
namespace re::Representation {

class Node : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::Node;
    explicit Node(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    types::Ipv4 ip_address_;
    std::vector<types::Uuid> deployed_container_uuids_;
    std::vector<types::Uuid> deployed_component_instance_uuids_;
    std::vector<types::Uuid> deployed_logging_client_uuids_;
    std::vector<types::Uuid> deployed_logging_server_uuids_;
};

} // namespace re::Representation
#endif // RE_NODE_H
