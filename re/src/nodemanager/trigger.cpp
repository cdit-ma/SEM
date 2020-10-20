#include "trigger.h"
#include "systeminfo.h"
#include <future>
#include <utility>
namespace re::NodeManager {

namespace detail {
auto ShouldFire(Trigger::Comparator comparator, double threshold_value, double current_value)
    -> bool
{
    switch(comparator) {
        case Trigger::Comparator::Greater:
            return current_value > threshold_value;
        case Trigger::Comparator::Less:
            return current_value < threshold_value;
        case Trigger::Comparator::GreaterOrEqual:
            return current_value >= threshold_value;
        case Trigger::Comparator::LessOrEqual:
            return current_value <= threshold_value;
        case Trigger::Comparator::Equal:
            return current_value == threshold_value;
        case Trigger::Comparator::NotEqual:
            return current_value != threshold_value;
    }
}
} // namespace detail

namespace conversions {
auto MetricInternalToProto(Trigger::Metric metric) -> ::NodeManager::TriggerPrototype_Metric
{
    switch(metric) {
        case Trigger::Metric::CpuUtil:
            return ::NodeManager::TriggerPrototype_Metric_CPU_UTIL;
        case Trigger::Metric::MemUtil:
            return ::NodeManager::TriggerPrototype_Metric_MEM_UTIL;
    }
    throw std::invalid_argument("Invalid metric found in trigger MetricInternalToProto");
}

auto ComparatorInternalToProto(Trigger::Comparator comparator)
    -> ::NodeManager::TriggerPrototype_Comparator
{
    switch(comparator) {
        case Trigger::Comparator::Equal:
            return ::NodeManager::TriggerPrototype_Comparator_EQUAL;
        case Trigger::Comparator::Greater:
            return ::NodeManager::TriggerPrototype_Comparator_GREATER;
        case Trigger::Comparator::Less:
            return ::NodeManager::TriggerPrototype_Comparator_LESS;
        case Trigger::Comparator::GreaterOrEqual:
            return ::NodeManager::TriggerPrototype_Comparator_GREATER_OR_EQUAL;
        case Trigger::Comparator::LessOrEqual:
            return ::NodeManager::TriggerPrototype_Comparator_LESS_OR_EQUAL;
        case Trigger::Comparator::NotEqual:
            return ::NodeManager::TriggerPrototype_Comparator_NOT_EQUAL;
    }
    throw std::invalid_argument("Invalid comparator found in trigger ComparatorInternalToProto");
}

auto MetricProtoToInternal(::NodeManager::TriggerPrototype_Metric metric) -> Trigger::Metric
{
    switch(metric) {
        case ::NodeManager::TriggerPrototype_Metric_CPU_UTIL:
            return Trigger::Metric::CpuUtil;
        case ::NodeManager::TriggerPrototype_Metric_MEM_UTIL:
            return Trigger::Metric::MemUtil;
        case ::NodeManager::
            TriggerPrototype_Metric_TriggerPrototype_Metric_INT_MIN_SENTINEL_DO_NOT_USE_:
        case ::NodeManager::
            TriggerPrototype_Metric_TriggerPrototype_Metric_INT_MAX_SENTINEL_DO_NOT_USE_:
        default:
            throw std::invalid_argument("Invalid metric type found in trigger prototype");
    }
}

auto ComparatorProtoToInternal(::NodeManager::TriggerPrototype_Comparator comparator)
    -> Trigger::Comparator
{
    switch(comparator) {
        case ::NodeManager::TriggerPrototype_Comparator_EQUAL:
            return Trigger::Comparator::Equal;
        case ::NodeManager::TriggerPrototype_Comparator_GREATER:
            return Trigger::Comparator::Greater;
        case ::NodeManager::TriggerPrototype_Comparator_LESS:
            return Trigger::Comparator::Less;
        case ::NodeManager::TriggerPrototype_Comparator_GREATER_OR_EQUAL:
            return Trigger::Comparator::GreaterOrEqual;
        case ::NodeManager::TriggerPrototype_Comparator_LESS_OR_EQUAL:
            return Trigger::Comparator::LessOrEqual;
        case ::NodeManager::
            TriggerPrototype_Comparator_TriggerPrototype_Comparator_INT_MIN_SENTINEL_DO_NOT_USE_:
        case ::NodeManager::
            TriggerPrototype_Comparator_TriggerPrototype_Comparator_INT_MAX_SENTINEL_DO_NOT_USE_:
        default:
            throw std::invalid_argument("Invalid comparator type found in trigger prototype");
    }
}

} // namespace conversions

using namespace detail;
using namespace conversions;

auto Trigger::Check() -> void
{
    auto current_value = GetCurrentMetricValue();
    if(ShouldFire(comparator_, threshold_value_, current_value)) {
        try {
            strategy_.Execute();
        } catch(const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            throw ex;
        }
    }
}

double Trigger::GetCurrentMetricValue()
{
    system_.Update();
    if(metric_ == Metric::CpuUtil) {
        return system_.GetOverallCpuUtilization(listener_id_);
    } else if(metric_ == Metric::MemUtil) {
        return system_.GetOverallMemUtilization(listener_id_);
    } else {
        throw std::invalid_argument("Unhandled metric type in GetCurrentMetricValue");
    }
}

Trigger::Trigger(std::string name,
                 std::string id,
                 Trigger::Metric metric,
                 Trigger::Comparator comparator,
                 double threshold_value,
                 Strategy strategy) :
    name_{std::move(name)},
    id_{std::move(id)},
    metric_{metric},
    comparator_{comparator},
    threshold_value_{threshold_value},
    strategy_{std::move(strategy)},
    listener_id_{system_.RegisterListener()}
{
}

Trigger::Trigger(const ::NodeManager::TriggerPrototype& trigger_pb, Strategy strategy) :
    Trigger(trigger_pb.info().name(),
            trigger_pb.info().id(),
            MetricProtoToInternal(trigger_pb.metric()),
            ComparatorProtoToInternal(trigger_pb.comparator()),
            trigger_pb.value(),
            std::move(strategy))
{
}

auto Trigger::PollLoop() -> void
{
    while(true) {
        std::unique_lock lock{poll_thread_mutex_};
        if(poll_thread_cv_.wait_for(lock, std::chrono::seconds(1),
                                    [this]() { return stop_poll_thread_.load(); })) {
            break;
        } else {
            Check();
        }
    }
}

auto Trigger::Terminate() -> void
{
    try {
        {
            std::lock_guard lock{poll_thread_mutex_};
            stop_poll_thread_ = true;
        }
        poll_thread_cv_.notify_all();
        if(poll_thread_handle_.valid()){
            poll_thread_handle_.wait_for(std::chrono::seconds(1));
        }
    } catch(const std::exception& ex) {
        std::cerr << "Exception caught in Trigger::Terminate" << std::endl;
    }
}
void Trigger::Start() {
    poll_thread_handle_ = std::async(std::launch::async, &Trigger::PollLoop, this);
}
} // namespace re::NodeManager
