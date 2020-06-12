#ifndef RE_PORTDELEGATEINSTANCE_H
#define RE_PORTDELEGATEINSTANCE_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <types/uuid.h>
namespace re::Representation {
class PortDelegateInstance : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::PortDelegateInstance;
    explicit PortDelegateInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    std::string medea_aggregate_type_;
    std::vector<types::Uuid> connected_port_delegate_uuids_;
    std::vector<types::Uuid> connected_port_instance_uuids_;
};
} // namespace re::Representation

#endif // RE_PORTDELEGATEINSTANCE_H
