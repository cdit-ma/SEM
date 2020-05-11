#ifndef RE_TRIGGERDEFINITION_H
#define RE_TRIGGERDEFINITION_H

#include "modelentity.h"
#include "graphmlparser.h"
#include "experimentdefinition.pb.h"
#include "uuid.h"
#include <chrono>
namespace re::Representation {

class TriggerDefinition : public DefaultModelEntity {
public:
    using PbType = re::network::protocol::experimentdefinition::TriggerDefinitionPrototype;
    TriggerDefinition(GraphmlParser& parser, const std::string& medea_id);
    enum class Metric { CpuUtil, MemoryUtil };
    enum class Comparator { Equal, Greater, Less, GreaterOrEqual, LessOrEqual, NotEqual };

    explicit TriggerDefinition(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
    [[nodiscard]] auto IsSingleActivation() const -> bool { return single_activation_; }
    [[nodiscard]] auto GetReactivationCooldown() const -> std::chrono::milliseconds
    {
        return std::chrono::milliseconds{reactivation_disabled_ms_};
    }

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
