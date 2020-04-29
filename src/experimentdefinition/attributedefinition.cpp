#include "attributedefinition.h"
namespace re::Representation {
auto TypeFromProto(const network::protocol::experimentdefinition::AttributeType& type)
    -> AttributeDefinition::Type
{
    switch(type) {
        case network::protocol::experimentdefinition::String:
            return AttributeDefinition::Type::String;
        case network::protocol::experimentdefinition::Integer:
            return AttributeDefinition::Type::Integer;
        case network::protocol::experimentdefinition::Character:
            return AttributeDefinition::Type::Character;
        case network::protocol::experimentdefinition::Boolean:
            return AttributeDefinition::Type::Boolean;
        case network::protocol::experimentdefinition::Double:
            return AttributeDefinition::Type::Double;
        case network::protocol::experimentdefinition::Float:
            return AttributeDefinition::Type::Float;
        case network::protocol::experimentdefinition::StringList:
            return AttributeDefinition::Type::StringList;
        case network::protocol::experimentdefinition::AttributeType_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::AttributeType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid attribute type found in AttributeDefinition::TypeFromProto");
}

auto TypeToProto(const AttributeDefinition::Type& type)
    -> network::protocol::experimentdefinition::AttributeType
{
    switch(type) {
        case AttributeDefinition::Type::Integer:
            return network::protocol::experimentdefinition::Integer;
        case AttributeDefinition::Type::Character:
            return network::protocol::experimentdefinition::Character;
        case AttributeDefinition::Type::Boolean:
            return network::protocol::experimentdefinition::Boolean;
        case AttributeDefinition::Type::Float:
            return network::protocol::experimentdefinition::Float;
        case AttributeDefinition::Type::Double:
            return network::protocol::experimentdefinition::Double;
        case AttributeDefinition::Type::String:
            return network::protocol::experimentdefinition::String;
        case AttributeDefinition::Type::StringList:
            return network::protocol::experimentdefinition::StringList;
    }
    throw std::invalid_argument("Invalid attribute type found in AttributeDefinition::TypeToProto");
}

AttributeDefinition::AttributeDefinition(const re::Representation::AttributeDefinition::PbType& pb) :
    DefaultModelEntity{pb.core_data()}, type_{TypeFromProto(pb.type())}
{
}

auto AttributeDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_type(TypeToProto(type_));
    return out;
}

} // namespace re::Representation
