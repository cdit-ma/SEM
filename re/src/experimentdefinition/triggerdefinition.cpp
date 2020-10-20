#include "triggerdefinition.h"
namespace re::Representation {

namespace detail {
auto ParseComparator(const std::string& comparator_str) -> TriggerDefinition::Comparator
{
    if(comparator_str == "==") {
        return TriggerDefinition::Comparator::Equal;
    } else if(comparator_str == "!=") {
        return TriggerDefinition::Comparator::NotEqual;
    } else if(comparator_str == "<") {
        return TriggerDefinition::Comparator::Less;
    } else if(comparator_str == ">") {
        return TriggerDefinition::Comparator::Greater;
    } else if(comparator_str == "<=") {
        return TriggerDefinition::Comparator::LessOrEqual;
    } else if(comparator_str == ">=") {
        return TriggerDefinition::Comparator::GreaterOrEqual;
    } else {
        throw std::runtime_error("Invalid comparator string: " + comparator_str);
    }
}
} // namespace detail
auto MetricFromProto(const network::protocol::experimentdefinition::TriggerMetric& pb)
    -> TriggerDefinition::Metric
{
    switch(pb) {
        case network::protocol::experimentdefinition::CPU_UTIL:
            return TriggerDefinition::Metric::CpuUtil;
        case network::protocol::experimentdefinition::MEM_UTIL:
            return TriggerDefinition::Metric::MemoryUtil;
        case network::protocol::experimentdefinition::TriggerMetric_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::TriggerMetric_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid trigger metric type found in Trigger::MetricFromProto");
}

auto MetricToProto(const TriggerDefinition::Metric& metric)
    -> network::protocol::experimentdefinition::TriggerMetric
{
    switch(metric) {
        case TriggerDefinition::Metric::CpuUtil:
            return network::protocol::experimentdefinition::CPU_UTIL;
        case TriggerDefinition::Metric::MemoryUtil:
            return network::protocol::experimentdefinition::MEM_UTIL;
    }
    throw std::invalid_argument("Invalid trigger metric type found in Trigger::MetricToProto");
}

auto ComparatorFromProto(
    const network::protocol::experimentdefinition::TriggerComparator& comparator)
    -> TriggerDefinition::Comparator
{
    switch(comparator) {
        case network::protocol::experimentdefinition::EQUAL:
            return TriggerDefinition::Comparator::Equal;
        case network::protocol::experimentdefinition::GREATER:
            return TriggerDefinition::Comparator::Greater;
        case network::protocol::experimentdefinition::LESS:
            return TriggerDefinition::Comparator::Less;
        case network::protocol::experimentdefinition::GREATER_OR_EQUAL:
            return TriggerDefinition::Comparator::GreaterOrEqual;
        case network::protocol::experimentdefinition::LESS_OR_EQUAL:
            return TriggerDefinition::Comparator::LessOrEqual;
        case network::protocol::experimentdefinition::NOT_EQUAL:
            return TriggerDefinition::Comparator::NotEqual;
        case network::protocol::experimentdefinition::TriggerComparator_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::TriggerComparator_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid comparator type found in Trigger::ComparatorFromProto");
}

auto ComparatorToProto(const TriggerDefinition::Comparator& comparator)
    -> network::protocol::experimentdefinition::TriggerComparator
{
    switch(comparator) {
        case TriggerDefinition::Comparator::Equal:
            return network::protocol::experimentdefinition::EQUAL;
        case TriggerDefinition::Comparator::Greater:
            return network::protocol::experimentdefinition::GREATER;
        case TriggerDefinition::Comparator::Less:
            return network::protocol::experimentdefinition::LESS;
        case TriggerDefinition::Comparator::GreaterOrEqual:
            return network::protocol::experimentdefinition::GREATER_OR_EQUAL;
        case TriggerDefinition::Comparator::LessOrEqual:
            return network::protocol::experimentdefinition::LESS_OR_EQUAL;
        case TriggerDefinition::Comparator::NotEqual:
            return network::protocol::experimentdefinition::NOT_EQUAL;
    }
}

TriggerDefinition::TriggerDefinition(const PbType& pb) : DefaultModelEntity{pb.core_data()}
{
    value_ = pb.value();
    single_activation_ = pb.single_activation();
    metric_ = MetricFromProto(pb.metric());
    comparator_ = ComparatorFromProto(pb.comparator());
    reactivation_disabled_ms_ = pb.reactivation_disabled_ms();
}

auto TriggerDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_value(value_);
    out->set_single_activation(single_activation_);
    out->set_metric(MetricToProto(metric_));
    out->set_comparator(ComparatorToProto(comparator_));
    out->set_reactivation_disabled_ms(reactivation_disabled_ms_);
    return out;
}

TriggerDefinition::TriggerDefinition(GraphmlParser& parser, const std::string& medea_id) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}}
{
    value_ = std::stod(parser.GetDataValue(medea_id, "value"));

    auto single_activation_str = parser.GetDataValue(medea_id, "single_activation");
    if(single_activation_str == "false") {
        single_activation_ = false;
    } else if(single_activation_str == "true") {
        single_activation_ = true;
    }

    comparator_ = detail::ParseComparator(parser.GetDataValue(medea_id, "condition"));

    auto metric_str = parser.GetDataValue(medea_id, "trigger_type");
    if(metric_str == "CPU_util") {
        metric_ = Metric::CpuUtil;
    } else if(metric_str == "Mem_util") {
        metric_ = Metric::MemoryUtil;
    }

    if(!single_activation_) {
        reactivation_disabled_ms_ = std::stoi(parser.GetDataValue(medea_id, "wait_period (ms)"));
    }
}

} // namespace re::Representation
