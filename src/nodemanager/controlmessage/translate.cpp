#include "translate.h"
#include "controlmessage.pb.h"
#include <iostream>


ATTRIBUTE_TYPE GetAttributeTypeFromPb(const NodeManager::Attribute& attr){
    switch(attr.kind()){
        case NodeManager::Attribute::INTEGER:
            return ATTRIBUTE_TYPE::INTEGER;
        case NodeManager::Attribute::BOOLEAN:
            return ATTRIBUTE_TYPE::BOOLEAN;
        case NodeManager::Attribute::STRING:
            return ATTRIBUTE_TYPE::STRING;
        case NodeManager::Attribute::DOUBLE:
            return ATTRIBUTE_TYPE::DOUBLE;
        case NodeManager::Attribute::FLOAT:
            return ATTRIBUTE_TYPE::FLOAT;
        case NodeManager::Attribute::CHARACTER:
            return ATTRIBUTE_TYPE::CHARACTER;
        case NodeManager::Attribute::STRINGLIST:
            return ATTRIBUTE_TYPE::STRINGLIST;
        default:
            break;
    }
    return ATTRIBUTE_TYPE::STRING;
}


std::weak_ptr<Attribute> SetAttributeFromPb(std::shared_ptr<Activatable> a, const NodeManager::Attribute& attr){
    const auto type = GetAttributeTypeFromPb(attr);
    const auto name = attr.info().name();
    
    if(a){
        //Try get/construct an Attribute with the type/name
        auto attribute = a->ConstructAttribute(type, name).lock();

        if(attribute){
            switch(type){
            case ATTRIBUTE_TYPE::INTEGER:
            {
                attribute->set_Integer(attr.i());
                break;
            }
            case ATTRIBUTE_TYPE::CHARACTER:
            {
                attribute->set_Character((char) attr.i());
                break;
            }
            case ATTRIBUTE_TYPE::BOOLEAN:
            {
                attribute->set_Boolean((bool) attr.i());
                break;
            }
            case ATTRIBUTE_TYPE::DOUBLE:
            {
                attribute->set_Double(attr.d());
                break;
            }
            case ATTRIBUTE_TYPE::FLOAT:
            {
                attribute->set_Float((float)attr.d());
                break;
            }
            case ATTRIBUTE_TYPE::STRING:
            {
                //Set using the first instance
                for(auto s: attr.s()){
                    attribute->set_String(s);
                    break;
                }
                break;
            }
            case ATTRIBUTE_TYPE::STRINGLIST:
            {
                //Clear first
                attribute->StringList().clear();

                //Set
                for(auto s: attr.s()){
                    attribute->StringList().push_back(s);
                }
                break;
            }
            default:
                std::cerr << "Cannot cast Type: " << NodeManager::Attribute_Kind_Name(attr.kind()) << std::endl;
                break; 
            }
        }
        return attribute;
    }
    return std::weak_ptr<Attribute>();
}