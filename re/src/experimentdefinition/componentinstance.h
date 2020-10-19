#ifndef RE_COMPONENTINSTANCE_H
#define RE_COMPONENTINSTANCE_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
#include <vector>

namespace re::Representation {

class MiddlewarePortInstance;
class PeriodicPortInstance;
class TriggerInstance;
class ComponentInstance : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::ComponentInstance;
    ComponentInstance(GraphmlParser& parser,
                      const std::string& medea_id,
                      const types::Uuid& definition_uuid);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    explicit ComponentInstance(const PbType& pb);
    auto AddMiddlewarePortInstance(const MiddlewarePortInstance& port_instance) -> void;
    auto AddPeriodicPortInstance(const PeriodicPortInstance& port_instance) -> void;
    auto AddTriggerInstance(const TriggerInstance& trigger_instance) -> void;
    [[nodiscard]] auto GetDefinitionUuid() const -> types::Uuid;
    [[nodiscard]] auto GetMedeaType() const -> std::string;
    [[nodiscard]] auto GetMiddlewarePortInstanceUuids() const -> std::vector<types::Uuid>
    {
        return middleware_port_instance_uuids_;
    }
    [[nodiscard]] auto GetPeriodicPortInstanceUuids() const -> std::vector<types::Uuid>
    {
        return periodic_port_instance_uuids_;
    }

    [[nodiscard]] auto GetWorkerInstanceUuids() const -> std::vector<types::Uuid>
    {
        return worker_instance_uuids_;
    }

    [[nodiscard]] auto GetAttributeInstanceUuids() const -> std::vector<types::Uuid>
    {
        return attribute_instance_uuids_;
    }

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
