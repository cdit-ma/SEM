#ifndef RE_STRATEGY_H
#define RE_STRATEGY_H

#include "modelentity.h"
#include <string>
#include <types/uuid.h>
namespace re::Representation {

class Strategy : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::StrategyPrototype;
    explicit Strategy(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    // Container to start on strategy execution.
    std::string container_medea_id_;
    std::string container_medea_name_;
};
} // namespace re::Representation

#endif // RE_STRATEGY_H
