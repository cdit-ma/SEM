#include "triggerinstance.h"
#include "triggerdefinition.h"
#include "strategy.h"
namespace re::Representation {

TriggerInstance::TriggerInstance(const PbType& pb) : DefaultModelEntity{pb.core_data()}
{
    definition_uuid_ = types::Uuid{pb.definition_uuid()};
    strategy_uuid_ = types::Uuid{pb.strategy_uuid()};
}
auto TriggerInstance::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_definition_uuid(definition_uuid_.to_string());
    out->set_strategy_uuid(strategy_uuid_.to_string());
    return out;
}
auto TriggerInstance::GetStrategyUuid() const -> types::Uuid
{
    return strategy_uuid_;
}
TriggerInstance::TriggerInstance(GraphmlParser& parser,
                                 const std::string& medea_id,
                                 const TriggerDefinition& definition) :
    DefaultModelEntity{{types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}},
    definition_uuid_{definition.GetCoreData().GetUuid()}
{
}
auto TriggerInstance::AddStrategy(const Strategy& strategy) -> void {
    strategy_uuid_ = strategy.GetCoreData().GetUuid();
}

} // namespace re::Representation
