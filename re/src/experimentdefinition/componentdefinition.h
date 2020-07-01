#ifndef RE_DEFINITION_COMPONENT_H
#define RE_DEFINITION_COMPONENT_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "uuid.h"
namespace re::Representation {

class ComponentDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::ComponentDefinition;
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit ComponentDefinition(const PbType& pb);

private:
    std::vector<std::string> medea_namespaces_;

    std::vector<types::Uuid> port_definition_uuids_;
    std::vector<types::Uuid> worker_definition_uuids_;
    std::vector<types::Uuid> attribute_definition_uuids_;
    std::vector<types::Uuid> trigger_definition_uuids_;
};

} // namespace re::Representation
#endif // RE_DEFINITION_COMPONENT_H
