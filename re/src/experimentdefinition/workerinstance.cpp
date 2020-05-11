#include "workerinstance.h"
#include "attributeinstance.h"
#include "workerdefinition.h"

namespace re::Representation {

WorkerInstance::WorkerInstance(const WorkerInstance::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    definition_uuid_ = types::Uuid{pb.definition_uuid()};

    for(const auto& attribute_instance_uuid : pb.attribute_instance_uuids()) {
        attribute_instance_uuids_.emplace_back(attribute_instance_uuid);
    }
}
auto WorkerInstance::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_definition_uuid(definition_uuid_.to_string());
    for(const auto& attribute_instance_uuid : attribute_instance_uuids_) {
        out->add_attribute_instance_uuids(attribute_instance_uuid.to_string());
    }
    return out;
}
WorkerInstance::WorkerInstance(GraphmlParser& parser,
                               const std::string& medea_id,
                               const WorkerDefinition& definition) :
    DefaultModelEntity{{types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}},
    definition_uuid_{definition.GetCoreData().GetUuid()}
{
}
void WorkerInstance::AddAttributeInstance(const AttributeInstanceInterface& attribute_instance)
{
    attribute_instance_uuids_.emplace_back(attribute_instance.GetCoreData().GetUuid());
}

} // namespace re::Representation