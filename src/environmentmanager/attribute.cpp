#include "attribute.h"

using namespace EnvironmentManager;

Attribute::Attribute(const NodeManager::Attribute& attribute){
    id_ = attribute.info().id();
    name_ = attribute.info().name();


    kind_ = ProtoAttributeKindToInternal(attribute.kind());

    switch(kind_){
        case Kind::String:
        case Kind::Stringlist:{
            for(const auto& string : attribute.s()){
                string_list_.push_back(string);
            }
            break;
        }

        case Kind::Float:
        case Kind::Double:{
            double_value_ = attribute.d();
            break;
        }

        case Kind::Character:{
            integer_value_ = (uint64_t) attribute.i();
            break;
        }

        case Kind::Integer:{
            integer_value_ = attribute.i();
            break;
        }

        case Kind::Boolean:{
            boolean_value_ = attribute.i() != 0;
            break;
        }
    }
}

NodeManager::Attribute* Attribute::GetUpdate(){
    NodeManager::Attribute* attribute;
    if(dirty_){
        attribute = new NodeManager::Attribute();

        attribute->mutable_info()->set_name(name_);
        attribute->mutable_info()->set_id(id_);

        attribute->set_kind(InternalAttributeKindToProto(kind_));

        switch(kind_){
            case Kind::String:
            case Kind::Stringlist:{
                for(const auto& string : string_list_){
                    attribute->add_s(string);
                }
                break;
            }
            case Kind::Float:
            case Kind::Double:{
                attribute->set_d(double_value_);
                break;
            }
            case Kind::Integer:
            case Kind::Character:{
                attribute->set_i(integer_value_);
                break;
            }
            case Kind::Boolean:{
                attribute->set_i(boolean_value_ ? 1 : 0);
                break;
            }
        }
    }
    return attribute;
}

NodeManager::Attribute* Attribute::GetProto(){
    NodeManager::Attribute* attribute = new NodeManager::Attribute();

    attribute->mutable_info()->set_name(name_);
    std::cout << name_ << std::endl;
    attribute->mutable_info()->set_id(id_);

    attribute->set_kind(InternalAttributeKindToProto(kind_));

    switch(kind_){
        case Kind::String:
        case Kind::Stringlist:{
            for(const auto& string : string_list_){
                attribute->add_s(string);
            }
            break;
        }
        case Kind::Float:
        case Kind::Double:{
            attribute->set_d(double_value_);
            break;
        }
        case Kind::Integer:
        case Kind::Character:{
            attribute->set_i(integer_value_);
            break;
        }
        case Kind::Boolean:{
            attribute->set_i(boolean_value_ ? 1 : 0);
            break;
        }
    }
    std::cout << attribute->DebugString() << std::endl;
    return attribute;
}

Attribute::Kind Attribute::ProtoAttributeKindToInternal(NodeManager::Attribute::Kind kind){
    switch(kind){
        case NodeManager::Attribute::STRING:{
            return Kind::String;
        }
        case NodeManager::Attribute::INTEGER:{
            return Kind::Integer;
        }
        case NodeManager::Attribute::CHARACTER:{
            return Kind::Character;
        }
        case NodeManager::Attribute::BOOLEAN:{
            return Kind::Boolean;
        }
        case NodeManager::Attribute::DOUBLE:{
            return Kind::Double;
        }
        case NodeManager::Attribute::FLOAT:{
            return Kind::Float;
        }
        case NodeManager::Attribute::STRINGLIST:{
            return Kind::Stringlist;
        }
        default:{
            return Kind::String;
        }
    }
}

NodeManager::Attribute::Kind Attribute::InternalAttributeKindToProto(Attribute::Kind kind){
    switch(kind){
        case Kind::String:{
            return NodeManager::Attribute::STRING;
        }
        case Kind::Integer:{
            return NodeManager::Attribute::INTEGER;
        }
        case Kind::Character:{
            return NodeManager::Attribute::CHARACTER;
        }
        case Kind::Boolean:{
            return NodeManager::Attribute::BOOLEAN;
        }
        case Kind::Double:{
            return NodeManager::Attribute::DOUBLE;
        }
        case Kind::Float:{
            return NodeManager::Attribute::FLOAT;
        }
        case Kind::Stringlist:{
            return NodeManager::Attribute::STRINGLIST;
        }
        default:{
            return NodeManager::Attribute::STRINGLIST;
        }
    }
}