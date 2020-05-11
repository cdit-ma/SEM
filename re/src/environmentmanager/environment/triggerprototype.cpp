#include "triggerprototype.h"

#include <utility>
namespace re::EnvironmentManager {

namespace conversions {
auto MetricInternalToProto(TriggerPrototype::Metric metric) -> NodeManager::TriggerPrototype_Metric
{
    switch(metric) {
        case TriggerPrototype::Metric::CpuUtil:
            return NodeManager::TriggerPrototype_Metric_CPU_UTIL;
        case TriggerPrototype::Metric::MemoryUtil:
            return NodeManager::TriggerPrototype_Metric_MEM_UTIL;
    }
}

auto ComparatorInternalToProto(TriggerPrototype::Comparator comparator)
    -> NodeManager::TriggerPrototype_Comparator
{
    switch(comparator) {
        case TriggerPrototype::Comparator::Equal:
            return NodeManager::TriggerPrototype_Comparator_EQUAL;
        case TriggerPrototype::Comparator::Greater:
            return NodeManager::TriggerPrototype_Comparator_GREATER;
        case TriggerPrototype::Comparator::Less:
            return NodeManager::TriggerPrototype_Comparator_LESS;
        case TriggerPrototype::Comparator::GreaterOrEqual:
            return NodeManager::TriggerPrototype_Comparator_GREATER_OR_EQUAL;
        case TriggerPrototype::Comparator::LessOrEqual:
            return NodeManager::TriggerPrototype_Comparator_LESS_OR_EQUAL;
    }
}

auto MetricProtoToInternal(NodeManager::TriggerPrototype_Metric metric) -> TriggerPrototype::Metric
{
    switch(metric) {
        case NodeManager::TriggerPrototype_Metric_CPU_UTIL:
            return TriggerPrototype::Metric::CpuUtil;
        case NodeManager::TriggerPrototype_Metric_MEM_UTIL:
            return TriggerPrototype::Metric::MemoryUtil;
        case NodeManager::TriggerPrototype_Metric_TriggerPrototype_Metric_INT_MIN_SENTINEL_DO_NOT_USE_:
        case NodeManager::TriggerPrototype_Metric_TriggerPrototype_Metric_INT_MAX_SENTINEL_DO_NOT_USE_:
        default:
            throw std::invalid_argument("Invalid metric type found in trigger prototype");
    }
}

auto ComparatorProtoToInternal(NodeManager::TriggerPrototype_Comparator comparator)
    -> TriggerPrototype::Comparator
{
    switch(comparator) {
        case NodeManager::TriggerPrototype_Comparator_EQUAL:
            return TriggerPrototype::Comparator::Equal;
        case NodeManager::TriggerPrototype_Comparator_GREATER:
            return TriggerPrototype::Comparator::Greater;
        case NodeManager::TriggerPrototype_Comparator_LESS:
            return TriggerPrototype::Comparator::Less;
        case NodeManager::TriggerPrototype_Comparator_GREATER_OR_EQUAL:
            return TriggerPrototype::Comparator::GreaterOrEqual;
        case NodeManager::TriggerPrototype_Comparator_LESS_OR_EQUAL:
            return TriggerPrototype::Comparator::LessOrEqual;
        case NodeManager::TriggerPrototype_Comparator_NOT_EQUAL:
            return TriggerPrototype::Comparator::NotEqual;
        case NodeManager::
            TriggerPrototype_Comparator_TriggerPrototype_Comparator_INT_MIN_SENTINEL_DO_NOT_USE_:
        case NodeManager::
            TriggerPrototype_Comparator_TriggerPrototype_Comparator_INT_MAX_SENTINEL_DO_NOT_USE_:
        default:
            throw std::invalid_argument("Invalid comparator type found in trigger prototype");
    }
}

auto StrategyProtoToInternal(const NodeManager::StrategyPrototype& strategy)
    -> TriggerPrototype::Strategy
{
    return TriggerPrototype::Strategy{strategy.info().id(), strategy.info().name(),
                                      strategy.container_id()};
}

auto StrategyInternalToProto(const TriggerPrototype::Strategy& strategy)
    -> std::unique_ptr<NodeManager::StrategyPrototype>
{
    auto strategy_proto = std::make_unique<NodeManager::StrategyPrototype>();
    strategy_proto->mutable_info()->set_name(strategy.strategy_name);
    strategy_proto->mutable_info()->set_id(strategy.strategy_id);
    strategy_proto->set_container_id(strategy.container_id);
    return strategy_proto;
}
} // namespace conversions

using namespace conversions;

TriggerPrototype::TriggerPrototype(Environment& environment,
                                   Component& parent,
                                   const NodeManager::TriggerPrototype& trigger) :
    environment_{environment}, parent_{parent}
{
    id_ = trigger.info().id();
    name_ = trigger.info().name();

    metric_ = MetricProtoToInternal(trigger.metric());
    comparator_ = ComparatorProtoToInternal(trigger.comparator());
    value_ = trigger.value();
    single_activation_ = trigger.single_activation();
    reactivation_disabled_ms_ = trigger.reactivation_disabled_ms();

    strategy_ = StrategyProtoToInternal(trigger.strategy());
}

auto TriggerPrototype::GetId() -> std::string
{
    return id_;
}

auto TriggerPrototype::GetName() -> std::string
{
    return name_;
}

auto TriggerPrototype::GetProto(const bool full_update)
    -> std::unique_ptr<NodeManager::TriggerPrototype>
{
    std::unique_ptr<NodeManager::TriggerPrototype> trigger;

    if(dirty_ || full_update) {
        trigger = std::make_unique<NodeManager::TriggerPrototype>();
        trigger->mutable_info()->set_name(name_);
        trigger->mutable_info()->set_id(id_);

        trigger->set_value(value_);
        trigger->set_single_activation(single_activation_);
        trigger->set_reactivation_disabled_ms(reactivation_disabled_ms_);

        trigger->set_metric(MetricInternalToProto(metric_));
        trigger->set_comparator(ComparatorInternalToProto(comparator_));

        trigger->set_allocated_strategy(StrategyInternalToProto(strategy_).release());
    }
    return trigger;
}
TriggerPrototype::TriggerPrototype(Environment& environment,
                                   Component& component,
                                   std::string id,
                                   std::string name,
                                   double value,
                                   TriggerPrototype::Metric metric,
                                   TriggerPrototype::Comparator comparator,
                                   bool single_activation,
                                   uint64_t reactivation_disabled_ms,
                                   TriggerPrototype::Strategy strategy) :
    environment_{environment},
    parent_{component},
    id_{std::move(id)},
    name_{std::move(name)},
    value_{value},
    single_activation_{single_activation},
    metric_{metric},
    comparator_{comparator},
    strategy_{std::move(strategy)},
    reactivation_disabled_ms_{reactivation_disabled_ms}
{
}

} // namespace re::EnvironmentManager
