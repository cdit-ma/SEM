#include "loggingserverdefinition.h"
namespace re::Representation {

LoggingServerDefinition::LoggingServerDefinition(
    const re::Representation::LoggingServerDefinition::PbType& pb) :
    DefaultModelEntity(pb.core_data())
{
    db_file_name_ = pb.db_file_name();
    for(const auto& client_uuid : pb.logging_client_uuids()) {
        logging_client_definition_uuids_.emplace_back(client_uuid);
    }
}
auto LoggingServerDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_db_file_name(db_file_name_);

    for(const auto& logging_client_definition_uuid : logging_client_definition_uuids_) {
        out->add_logging_client_uuids(logging_client_definition_uuid.to_string());
    }
    return out;
}
} // namespace re::Representation
