#include "strategy.h"
#include "container.h"
#include "graphmlparser.h"
namespace re::Representation {

Strategy::Strategy(const PbType& pb) :
    DefaultModelEntity{pb.core_data()},
    container_uuid_{pb.container_uuid()},
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
    out->set_container_uuid(container_uuid_.to_string());
    return out;
}
auto Strategy::GetContainerUuid() const -> sem::types::Uuid
{
    return container_uuid_;
}
Strategy::Strategy(GraphmlParser& parser, const std::string& medea_id, const Container& container) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}},
    container_uuid_{container.GetCoreData().GetUuid()},
    container_medea_id_{container.GetCoreData().GetMedeaId()},
    container_medea_name_{container.GetCoreData().GetMedeaName()}
{
}
} // namespace re::Representation
