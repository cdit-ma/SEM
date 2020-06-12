#ifndef RE_TRIGGERDEFINITION_H
#define RE_TRIGGERDEFINITION_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include <types/uuid.h>
namespace re::Representation {

class TriggerDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::TriggerDefinitionPrototype;
    enum class Metric { CpuUtil, MemoryUtil };
    enum class Comparator { Equal, Greater, Less, GreaterOrEqual, LessOrEqual, NotEqual };

    explicit TriggerDefinition(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    double value_;
    bool single_activation_;
    Metric metric_;
    Comparator comparator_;

    // Reactivation cooldown
    uint64_t reactivation_disabled_ms_;
};
} // namespace re::Representation

#endif // RE_TRIGGERDEFINITION_H
