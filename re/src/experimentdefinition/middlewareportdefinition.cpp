#include "middlewareportdefinition.h"

namespace re::Representation {

auto KindFromProto(const MiddlewarePortDefinition::MiddlewarePortKindPb& kind)
    -> MiddlewarePortDefinition::Kind
{
    switch(kind) {
        case network::protocol::experimentdefinition::Publisher:
            return MiddlewarePortDefinition::Kind::Publisher;
        case network::protocol::experimentdefinition::Subscriber:
            return MiddlewarePortDefinition::Kind::Subscriber;
        case network::protocol::experimentdefinition::Requester:
            return MiddlewarePortDefinition::Kind::Requester;
        case network::protocol::experimentdefinition::Replier:
            return MiddlewarePortDefinition::Kind::Replier;
        case network::protocol::experimentdefinition::MiddlewarePortKind_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::MiddlewarePortKind_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid port kind found in experimentdefinition::port proto -> "
                                "kind");
}

auto KindToProto(const MiddlewarePortDefinition::Kind& kind)
    -> MiddlewarePortDefinition::MiddlewarePortKindPb
{
    switch(kind) {
        case MiddlewarePortDefinition::Kind::Publisher:
            return network::protocol::experimentdefinition::Publisher;
        case MiddlewarePortDefinition::Kind::Subscriber:
            return network::protocol::experimentdefinition::Subscriber;
        case MiddlewarePortDefinition::Kind::Requester:
            return network::protocol::experimentdefinition::Requester;
        case MiddlewarePortDefinition::Kind::Replier:
            return network::protocol::experimentdefinition::Replier;
    }
    throw std::invalid_argument("Invalid port kind found in experimentdefinition::port kind -> "
                                "proto");
}

MiddlewarePortDefinition::MiddlewarePortDefinition(const MiddlewarePortDefinition::PbType& pb) :
    DefaultModelEntity{pb.core_data()}
{
    kind_ = KindFromProto(pb.kind());
    medea_aggregate_type_ = pb.medea_aggregate_type();

    for(const auto& attribute_definition_uuid : pb.attribute_definition_uuids()) {
        attribute_definition_uuids_.emplace_back(attribute_definition_uuid);
    }
}

auto MiddlewarePortDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_kind(KindToProto(kind_));
    out->set_medea_aggregate_type(medea_aggregate_type_);

    for(const auto& attribute_definition_uuid : attribute_definition_uuids_) {
        out->add_attribute_definition_uuids(attribute_definition_uuid.to_string());
    }
    return out;
}
} // namespace re::Representation