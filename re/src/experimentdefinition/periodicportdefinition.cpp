#include "periodicportdefinition.h"
#include "attributedefinition.h"
#include "middlewareportdefinition.h"

namespace re::Representation {

PeriodicPortDefinition::PeriodicPortDefinition(const PeriodicPortDefinition::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    frequency_attribute_definition_uuid_ = sem::types::Uuid{pb.frequency_attribute_definition_uuid()};
}
auto PeriodicPortDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_frequency_attribute_definition_uuid(frequency_attribute_definition_uuid_.to_string());

    return out;
}

PeriodicPortDefinition::PeriodicPortDefinition(GraphmlParser& parser,
                                               const std::string& medea_id,
                                               const AttributeDefinition& period_attribute) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}},
    frequency_attribute_definition_uuid_{period_attribute.GetCoreData().GetUuid()}
{
}
} // namespace re::Representation
