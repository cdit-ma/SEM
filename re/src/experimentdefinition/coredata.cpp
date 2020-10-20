#include "coredata.h"

namespace re::Representation {

auto CoreData::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_uuid(uuid_.to_string());
    out->set_medea_name(medea_name_);
    out->set_medea_id(medea_id_);

    return out;
}
CoreData::CoreData(const CoreData::PbType& pb)
{
    uuid_ = sem::types::Uuid{pb.uuid()};
    medea_name_ = pb.medea_name();
    medea_id_ = pb.medea_id();
}
CoreData::CoreData(sem::types::Uuid uuid, const std::string& medea_id, const std::string& medea_name) :
    uuid_{uuid}, medea_id_{medea_id}, medea_name_{medea_name}
{
}
} // namespace re::Representation