#ifndef RE_STRATEGY_H
#define RE_STRATEGY_H

#include "modelentity.h"
#include <string>
#include "uuid.h"
#include "graphmlparser.h"
namespace re::Representation {

class Container;
class Strategy : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::StrategyPrototype;
    explicit Strategy(const PbType& pb);
    Strategy(GraphmlParser& parser, const std::string& medea_id, const Container& container);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    [[nodiscard]] auto GetContainerUuid() const -> sem::types::Uuid;

private:
    // Container to start on strategy execution.
    sem::types::Uuid container_uuid_;
    std::string container_medea_id_;
    std::string container_medea_name_;
};
} // namespace re::Representation

#endif // RE_STRATEGY_H
