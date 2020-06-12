#include "workerdefinition.h"
namespace re::Representation {

WorkerDefinition::WorkerDefinition(const WorkerDefinition::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    cpp_class_name_ = pb.cpp_class_name();

    for(const auto& attribute_definition_uuid : pb.attribute_definition_uuids()) {
        attribute_definition_uuids_.emplace_back(attribute_definition_uuid);
    }
}
auto WorkerDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_cpp_class_name(cpp_class_name_);
    for(const auto& attribute_definition_uuid : attribute_definition_uuids_) {
        out->add_attribute_definition_uuids(attribute_definition_uuid.to_string());
    }
    return out;
}
} // namespace re::Representation
