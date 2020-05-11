#ifndef RE_PORTDELEGATEINSTANCE_H
#define RE_PORTDELEGATEINSTANCE_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
namespace re::Representation {

class MiddlewarePortInstance;
class PortDelegateInstance : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::PortDelegateInstance;
    explicit PortDelegateInstance(const PbType& pb);
    PortDelegateInstance(GraphmlParser& parser, const std::string& medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

    auto AddConnectedMiddlewarePortInstance(const MiddlewarePortInstance& middleware_port_instance)
        -> void;

    auto AddConnectedPortDelegate(const PortDelegateInstance& port_delegate_instance) -> void;

private:
    std::string medea_aggregate_type_;
    std::vector<types::Uuid> connected_port_delegate_uuids_;
    std::vector<types::Uuid> connected_port_instance_uuids_;
};
} // namespace re::Representation

#endif // RE_PORTDELEGATEINSTANCE_H
