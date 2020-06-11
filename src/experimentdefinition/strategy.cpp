#include "strategy.h"
namespace re::Representation {

Strategy::Strategy(const PbType& pb) :
    DefaultModelEntity{pb.core_data()},
    container_medea_name_{pb.container_medea_name()},
    container_medea_id_{pb.container_medea_id()}
{
}

auto Strategy::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_container_medea_id(container_medea_id_);
    out->set_container_medea_name(container_medea_name_);
    return out;
}
} // namespace re::Representation
