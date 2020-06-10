#include "periodicportinstance.h"
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

} // namespace re::Representation
