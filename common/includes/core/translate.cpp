#include "translate.h"
#include "controlmessage.pb.h"
#include <iostream>


::Attribute* set_attribute_from_pb(NodeManager::Attribute* attr, ::Attribute* a){
    bool set_type = a == 0;
    if(a == 0){
        //Construct!
        a = ::new Attribute();
        a->name = attr->name();

    }

    

    switch(attr->type()){
        case NodeManager::Attribute::INTEGER:{
            if(set_type){
                a->type = AT_INTEGER;
            }
            a->i = attr->i();
            break;
        }
        case NodeManager::Attribute::BOOLEAN:{
            if(set_type){
                a->type = AT_BOOLEAN;
            }
            a->i = (bool) attr->i();
            break;
        }
        case NodeManager::Attribute::DOUBLE:{
            if(set_type){
                a->type = AT_DOUBLE;
            }
            a->d = attr->d();
            break;
        }
        case NodeManager::Attribute::STRING:{
            if(set_type){
                a->type = AT_STRING;
            }
            for(auto s: attr->s()){
                a->s.push_back(s);
                break;
            }
            break;
        }
        case NodeManager::Attribute::STRINGLIST:{
            if(set_type){
                a->type = AT_STRINGLIST;
            }
            for(auto s: attr->s()){
                a->s.push_back(s);
            }
            break;
        }
        default:
            std::cout << "Cannot cast Type: " << NodeManager::Attribute_Type_Name(attr->type()) << std::endl;
            break;
    }
    return a;
}