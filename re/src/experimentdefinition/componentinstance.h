#ifndef RE_COMPONENTINSTANCE_H
#define RE_COMPONENTINSTANCE_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <types/uuid.h>
#include <vector>

namespace re::Representation {

class ComponentInstance : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::ComponentInstance;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit ComponentInstance(const PbType& pb);

private:
    types::Uuid definition_uuid_;
    std::string medea_type_;

    std::vector<types::Uuid> middleware_port_instance_uuids_;
    std::vector<types::Uuid> periodic_port_instance_uuids_;
    std::vector<types::Uuid> worker_instance_uuids_;
    std::vector<types::Uuid> attribute_instance_uuids_;
    std::vector<types::Uuid> trigger_instance_uuids_;
};
} // namespace re::Representation

#endif // RE_COMPONENTINSTANCE_H
