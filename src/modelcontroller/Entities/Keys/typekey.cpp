#include "typekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>
#include "namespacekey.h"

TypeKey::TypeKey(): Key("type", QVariant::String){
    //Combine Namespace and Type
    combine_namespace_kinds = {NODE_KIND::AGGREGATE, NODE_KIND::COMPONENT, NODE_KIND::ENUM};
}

QVariant TypeKey::validateDataChange(Data* data, QVariant data_value){
    QString desired_type = data_value.toString();
    QString new_type = desired_type;

    auto entity = data->getParent();


    if(!entity){
        return new_type;
    }
    auto node_kind = entity->isNode() ? ((Node*)entity)->getNodeKind() : NODE_KIND::NONE;

    if(combine_namespace_kinds.contains(node_kind)){
        auto namespace_data = entity->getData("namespace");
        auto label_data = entity->getData("label");

        auto namespace_value = namespace_data ? namespace_data->getValue().toString() : "";
        auto label_value = label_data ? label_data->getValue().toString() : "";

        new_type = NamespaceKey::CombineNamespaces(namespace_value, label_value);
    }else{
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
    }
    
    return new_type;
}


void TypeKey::BindInnerAndOuterTypes(Node* src, Node* dst, bool bind){
    auto src_inner_type_data = src->getData("inner_type");
    auto src_outer_type_data = src->getData("outer_type");
    auto src_type_data = src->getData("type");

    auto dst_inner_type_data = dst->getData("inner_type");
    auto dst_outer_type_data = dst->getData("outer_type");
    auto dst_type_data = dst->getData("type");

    //Got fully described data
    if(src_inner_type_data && src_outer_type_data && src_type_data){
        if(src_inner_type_data && dst_inner_type_data){
            src_inner_type_data->linkData(dst_inner_type_data, bind);
        }
        if(src_outer_type_data && dst_outer_type_data){
            src_outer_type_data->linkData(dst_outer_type_data, bind);
        }
        if(src_type_data && dst_type_data){
            src_type_data->linkData(dst_type_data, bind);
        }
    }else if(src_type_data){
        if(src_type_data && dst_inner_type_data){
            src_type_data->linkData(dst_inner_type_data, bind);
        }
    }
}

void TypeKey::BindNamespaceAndLabelToType(Node* node, bool bind){
    auto namespace_data = node->getData("namespace");
    auto label_data = node->getData("label");
    auto type_data = node->getData("type");
    if(namespace_data && label_data && type_data){
        namespace_data->linkData(type_data, bind);
        label_data->linkData(type_data, bind);
    }
}

void TypeKey::BindTypes(Node* src, Node* dst, bool bind){
    auto src_type_data = src->getData("type");
    auto dst_type_data = dst->getData("type");
    
    if(src_type_data && dst_type_data){
        src_type_data->linkData(dst_type_data, bind);
    }
}