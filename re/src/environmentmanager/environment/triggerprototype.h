#ifndef RE_TRIGGER_PROTOTYPE_HPP
#define RE_TRIGGER_PROTOTYPE_HPP

#include <proto/controlmessage/controlmessage.pb.h>

namespace re::EnvironmentManager {
class Environment;
class Component;
class TriggerPrototype {
public:
    enum class Metric { CpuUtil, MemoryUtil };
    enum class Comparator { Equal, Greater, Less, GreaterOrEqual, LessOrEqual, NotEqual };
    struct Strategy {
        std::string strategy_id;
        std::string strategy_name;
        std::string container_id;
    };
    TriggerPrototype(Environment& environment,
                     Component& component,
                     const NodeManager::TriggerPrototype& trigger_prototype);

    TriggerPrototype(Environment& environment,
                     Component& component,
                     std::string id,
                     std::string name,
                     double value,
                     Metric metric,
                     Comparator comparator,
                     bool single_activation,
                     uint64_t reactivation_disabled_ms,
                     Strategy strategy);

    std::string GetId();
    std::string GetName();
    auto GetProto(bool full_update) -> std::unique_ptr<NodeManager::TriggerPrototype>;

private:
    Environment& environment_;
    Component& parent_;
    bool dirty_;

    // Graphml details
    std::string id_;
    std::string name_;

    // Trigger details
    double value_;
    bool single_activation_;
    Metric metric_;
    Comparator comparator_;
    Strategy strategy_;

    // Reactivation cooldown
    uint64_t reactivation_disabled_ms_;
};
} // namespace re::EnvironmentManager

#endif // RE_TRIGGER_PROTOTYPE_HPP
