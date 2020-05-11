#include "attribute.h"

namespace re::EnvironmentManager {

Attribute::Attribute(const NodeManager::Attribute& attribute)
{
    id_ = attribute.info().id();
    name_ = attribute.info().name();

    kind_ = ProtoAttributeKindToInternal(attribute.kind());

    switch(kind_) {
        case Kind::String:
        case Kind::Stringlist: {
            for(const auto& string : attribute.s()) {
                string_list_.push_back(string);
            }
            break;
        }

        case Kind::Float:
        case Kind::Double: {
            double_value_ = attribute.d();
            break;
        }

        case Kind::Character: {
            integer_value_ = (uint64_t)attribute.i();
            break;
        }

        case Kind::Integer: {
            integer_value_ = attribute.i();
            break;
        }

        case Kind::Boolean: {
            boolean_value_ = attribute.i() != 0;
            break;
        }
    }
}

std::string Attribute::GetName() const
{
    return name_;
}

std::unique_ptr<NodeManager::Attribute> Attribute::GetProto(const bool full_update)
{
    std::unique_ptr<NodeManager::Attribute> attribute;

    if(full_update || dirty_) {
        attribute = std::unique_ptr<NodeManager::Attribute>(new NodeManager::Attribute());
        attribute->mutable_info()->set_name(name_);
        attribute->mutable_info()->set_id(id_);

        attribute->set_kind(InternalAttributeKindToProto(kind_));

        switch(kind_) {
            case Kind::String:
            case Kind::Stringlist: {
                for(const auto& string : string_list_) {
                    attribute->add_s(string);
                }
                break;
            }
            case Kind::Float:
            case Kind::Double: {
                attribute->set_d(double_value_);
                break;
            }
            case Kind::Integer:
            case Kind::Character: {
                attribute->set_i(integer_value_);
                break;
            }
            case Kind::Boolean: {
                attribute->set_i(boolean_value_ ? 1 : 0);
                break;
            }
            default: {
                throw std::runtime_error("Unhandled Attribute Kind");
            }
        }

        if(dirty_) {
            // clear dirty
            dirty_ = false;
        }
    }
    return attribute;
}

Attribute::Kind Attribute::ProtoAttributeKindToInternal(NodeManager::Attribute::Kind kind)
{
    switch(kind) {
        case NodeManager::Attribute::STRING: {
            return Kind::String;
        }
        case NodeManager::Attribute::INTEGER: {
            return Kind::Integer;
        }
        case NodeManager::Attribute::CHARACTER: {
            return Kind::Character;
        }
        case NodeManager::Attribute::BOOLEAN: {
            return Kind::Boolean;
        }
        case NodeManager::Attribute::DOUBLE: {
            return Kind::Double;
        }
        case NodeManager::Attribute::FLOAT: {
            return Kind::Float;
        }
        case NodeManager::Attribute::STRINGLIST: {
            return Kind::Stringlist;
        }
        default: {
            return Kind::String;
        }
    }
}

NodeManager::Attribute::Kind Attribute::InternalAttributeKindToProto(Attribute::Kind kind)
{
    switch(kind) {
        case Kind::String: {
            return NodeManager::Attribute::STRING;
        }
        case Kind::Integer: {
            return NodeManager::Attribute::INTEGER;
        }
        case Kind::Character: {
            return NodeManager::Attribute::CHARACTER;
        }
        case Kind::Boolean: {
            return NodeManager::Attribute::BOOLEAN;
        }
        case Kind::Double: {
            return NodeManager::Attribute::DOUBLE;
        }
        case Kind::Float: {
            return NodeManager::Attribute::FLOAT;
        }
        case Kind::Stringlist: {
            return NodeManager::Attribute::STRINGLIST;
        }
        default: {
            return NodeManager::Attribute::STRINGLIST;
        }
    }
}
Attribute::Attribute(const types::Uuid& event_uuid,
                     const Representation::AttributeInstancePb& pb)
{
    id_ = pb.core_data().medea_id() + "_" + event_uuid.to_string();
    name_ = pb.core_data().medea_name();
    switch(pb.type()) {
        case network::protocol::experimentdefinition::String:
            kind_ = Kind::String;
            break;
        case network::protocol::experimentdefinition::Integer:
            kind_ = Kind::Integer;
            integer_value_ = pb.i();
            break;
        case network::protocol::experimentdefinition::Character:
            kind_ = Kind::Character;
            integer_value_ = pb.c();
            break;
        case network::protocol::experimentdefinition::Boolean:
            kind_ = Kind::Boolean;
            boolean_value_ = pb.b();
            break;
        case network::protocol::experimentdefinition::Double:
            kind_ = Kind::Double;
            double_value_ = pb.d();
            break;
        case network::protocol::experimentdefinition::Float:
            kind_ = Kind::Float;
            double_value_ = pb.f();
            break;
        case network::protocol::experimentdefinition::StringList:
            kind_ = Kind::Stringlist;
            break;
        case network::protocol::experimentdefinition::AttributeType_INT_MIN_SENTINEL_DO_NOT_USE_:
        case network::protocol::experimentdefinition::AttributeType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
}
} // namespace re::EnvironmentManager