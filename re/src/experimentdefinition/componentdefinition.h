#ifndef RE_DEFINITION_COMPONENT_H
#define RE_DEFINITION_COMPONENT_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
namespace re::Representation {

class PeriodicPortDefinition;
class MiddlewarePortDefinition;
class ComponentDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::ComponentDefinition;
    ComponentDefinition(GraphmlParser& parser, const std::string& medea_id);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit ComponentDefinition(const PbType& pb);
    auto AddMiddlewarePortDefinition(const MiddlewarePortDefinition& port_definition) -> void;
    auto AddPeriodicPortDefinition(const PeriodicPortDefinition& port_definition) -> void;

private:
    std::vector<std::string> medea_namespaces_;

    std::vector<types::Uuid> middleware_port_definition_uuids_;
    std::vector<types::Uuid> periodic_port_definition_uuids_;
    std::vector<types::Uuid> worker_definition_uuids_;
    std::vector<types::Uuid> attribute_definition_uuids_;
    std::vector<types::Uuid> trigger_definition_uuids_;
};

} // namespace re::Representation
#endif // RE_DEFINITION_COMPONENT_H
