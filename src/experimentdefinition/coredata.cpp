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
    uuid_ = types::Uuid{pb.uuid()};
    medea_name_ = pb.medea_name();
    medea_id_ = pb.medea_id();
}
} // namespace re::Representation