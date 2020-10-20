#ifndef RE_PERIODICPORTINSTANCE_H
#define RE_PERIODICPORTINSTANCE_H

#include "modelentity.h"
#include <chrono>
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
#include <vector>

namespace re::Representation {

class AttributeInstanceInterface;
class PeriodicPortDefinition;
class PeriodicPortInstance : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::PeriodicPortInstance;
    explicit PeriodicPortInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    PeriodicPortInstance(GraphmlParser& parser,
                         const std::string& medea_id,
                         const PeriodicPortDefinition& definition,
                         const AttributeInstanceInterface& frequency_attribute);

    [[nodiscard]] auto GetFrequencyAttributeUuid() const -> sem::types::Uuid
    {
        return frequency_attribute_uuid_;
    }

private:
    sem::types::Uuid definition_uuid_;
    sem::types::Uuid frequency_attribute_uuid_;
};
} // namespace re::Representation

#endif // RE_PERIODICPORTINSTANCE_H
