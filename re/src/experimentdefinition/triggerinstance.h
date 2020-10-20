#ifndef RE_TRIGGERINSTANCE_H
#define RE_TRIGGERINSTANCE_H

#include "modelentity.h"
#include "graphmlparser.h"
#include <string>
#include "uuid.h"
namespace re::Representation {

class TriggerDefinition;
class Strategy;
class TriggerInstance : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::TriggerInstancePrototype;
    explicit TriggerInstance(const PbType& pb);
    TriggerInstance(GraphmlParser& parser,
                    const std::string& medea_id,
                    const TriggerDefinition& definition);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    auto GetStrategyUuid() const -> sem::types::Uuid;
    auto AddStrategy(const Strategy& strategy) -> void;
    auto GetDefinitionUuid() const -> sem::types::Uuid { return definition_uuid_; }

private:
    sem::types::Uuid definition_uuid_;

    sem::types::Uuid strategy_uuid_;
};
} // namespace re::Representation

#endif // RE_TRIGGERINSTANCE_H
