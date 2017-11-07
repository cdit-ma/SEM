#include "translate.h"
#include "controlmessage.pb.h"
#include <iostream>


ATTRIBUTE_TYPE GetAttributeTypeFromPb(NodeManager::Attribute* attr){
    if(attr){
        switch(attr->kind()){
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
    }
    return ATTRIBUTE_TYPE::STRING;
}


std::shared_ptr<::Attribute> SetAttributeFromPb(NodeManager::Attribute* attr, std::shared_ptr<::Attribute> a){
    
    auto type = GetAttributeTypeFromPb(attr);
    
    bool set_type = a == 0;
    if(a == 0){
        a = std::make_shared<Attribute>(type, attr->mutable_info()->name());
    }
    switch(type){
        case ATTRIBUTE_TYPE::INTEGER:
        {
            a->set_Integer(attr->i());
            break;
        }
        case ATTRIBUTE_TYPE::CHARACTER:
        {
            a->set_Character((char) attr->i());
            break;
        }
        case ATTRIBUTE_TYPE::BOOLEAN:
        {
            a->set_Boolean((bool) attr->i());
            break;
        }
        case ATTRIBUTE_TYPE::DOUBLE:
        {
            a->set_Double(attr->d());
            break;
        }
        case ATTRIBUTE_TYPE::FLOAT:
        {
            a->set_Float((float)attr->d());
            break;
        }
        case ATTRIBUTE_TYPE::STRING:
        {
            //Set using the first instance
            for(auto s: attr->s()){
                a->set_String(s);
                break;
            }
            break;
        }
        case ATTRIBUTE_TYPE::STRINGLIST:
        {
            //Clear first
            a->StringList().clear();

            //Set
            for(auto s: attr->s()){
                a->StringList().push_back(s);
            }
            break;
        }
        default:
            std::cout << "Cannot cast Type: " << NodeManager::Attribute_Kind_Name(attr->kind()) << std::endl;
            break; 
    }
    return a;
}