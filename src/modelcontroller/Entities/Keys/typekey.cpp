#include "typekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>

TypeKey::TypeKey(): Key("type", QVariant::String){

}

QVariant TypeKey::validateDataChange(Data* data, QVariant data_value){
    QString desired_type = data_value.toString();
    QString new_type = desired_type;

    auto entity = data->getParent();

    if(!entity){
        return new_type;
    }

    auto inner_type_data = entity->getData("inner_type");
    auto outer_type_data = entity->getData("outer_type");

    //Check for life!
    auto inner_type = inner_type_data ? inner_type_data->getValue().toString() : "";
    auto outer_type = outer_type_data ? outer_type_data->getValue().toString() : "";

    if(outer_type.size()){
        new_type = outer_type + "<" + inner_type + ">";
    }else if(inner_type.size()){
        new_type = inner_type;
    }
    return new_type;
}


bool TypeKey::BindTypes(Node* src, Node* dst, bool bind){
    auto src_inner_type_data = src->getData("inner_type");
    auto src_outer_type_data = src->getData("outer_type");
    auto src_type_data = src->getData("type");

    auto dst_inner_type_data = dst->getData("inner_type");
    auto dst_outer_type_data = dst->getData("outer_type");
    auto dst_type_data = dst->getData("type");

    //Got fully described data
    if(src_inner_type_data && src_outer_type_data && src_type_data){
        if(src_inner_type_data && dst_inner_type_data){
            src_inner_type_data->bindData(dst_inner_type_data, bind);
        }
        if(src_outer_type_data && dst_outer_type_data){
            src_outer_type_data->bindData(dst_outer_type_data, bind);
        }
        if(src_type_data && dst_type_data){
            src_type_data->bindData(dst_type_data, bind);
        }
    }else if(src_type_data){
        if(src_type_data && dst_inner_type_data){
            src_type_data->bindData(dst_inner_type_data, bind);
        }
    }
    
    return true;
}
