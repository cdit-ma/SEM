#ifndef RE_TRIGGERINSTANCE_H
#define RE_TRIGGERINSTANCE_H

#include "modelentity.h"
#include <string>
#include <types/uuid.h>
namespace re::Representation {

class TriggerInstance : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::TriggerInstancePrototype;
    explicit TriggerInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    types::Uuid definition_uuid_;

    types::Uuid strategy_uuid_;
};
} // namespace re::Representation

#endif // RE_TRIGGERINSTANCE_H
