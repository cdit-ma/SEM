#include "periodicportinstance.h"
#include "periodicportdefinition.h"
#include "attributeinstance.h"
#include "middlewareportinstance.h"

namespace re::Representation {

PeriodicPortInstance::PeriodicPortInstance(const PeriodicPortInstance::PbType& pb) :
    DefaultModelEntity{pb.core_data()},
    definition_uuid_{pb.definition_uuid()},
    frequency_attribute_uuid_{pb.frequency_attribute_instance_uuid()}
{
}
auto PeriodicPortInstance::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_definition_uuid(definition_uuid_.to_string());
    out->set_frequency_attribute_instance_uuid(frequency_attribute_uuid_.to_string());
    return out;
}
PeriodicPortInstance::PeriodicPortInstance(GraphmlParser& parser,
                                           const std::string& medea_id,
                                           const PeriodicPortDefinition& definition,
                                           const AttributeInstanceInterface& frequency_attribute) :
    DefaultModelEntity{{types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}}
{
    definition_uuid_ = definition.GetCoreData().GetUuid();
    frequency_attribute_uuid_ = frequency_attribute.GetCoreData().GetUuid();
}

} // namespace re::Representation
