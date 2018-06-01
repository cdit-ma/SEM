#include "typekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>
#include "namespacekey.h"

TypeKey::TypeKey(EntityFactoryBroker& broker): Key(broker, "type", QVariant::String){
    //Combine Namespace and Type
    combine_namespace_kinds = {NODE_KIND::AGGREGATE, NODE_KIND::COMPONENT, NODE_KIND::ENUM};
}

QList<QVariant> TypeKey::GetValidPrimitiveTypes(){
    QList<QVariant> vals;

    for(const auto& val : GetPrimitiveTypes()){
        vals += val;
    }
    std::sort(vals.begin(), vals.end());
    return vals;
}

QVariant TypeKey::GetDefaultPrimitiveType(){
    return "String";
}

QList<QVariant> TypeKey::GetValidNumberTypes(){
    QList<QVariant> vals;
    for(const auto& val : GetNumberTypes()){
        vals += val;
    }
    return vals;
}



QSet<QString> TypeKey::GetPrimitiveTypes(){
    return {"String", "Boolean", "Integer", "Double", "Float", "Character"};
}

QSet<QString> TypeKey::GetNumberTypes(){
    return {"Integer", "Double", "Float", "Character"};
}

QString TypeKey::GetCPPPrimitiveType(const QString& type){
    if(type == "Boolean"){
        return "bool";
    }else if(type == "Double"){
        return "double";
    }else if(type == "Float"){
        return "float";
    }else if(type == "Integer"){
        return "int";
    }else if(type == "Character"){
        return "char";
    }
    return type;
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

        if(inner_type_data && outer_type_data){
            auto inner_type = inner_type_data->getValue().toString();
            auto outer_type = outer_type_data->getValue().toString();
            
            if(outer_type.size()){
                new_type = outer_type + "<" + inner_type + ">";
            }else if(inner_type.size()){
                new_type = inner_type;
            }else{
                new_type = "";
            }
        }
        
    }
    return Key::validateDataChange(data, new_type);
}

#include <QDebug>

void TypeKey::BindInnerAndOuterTypes(Node* src, Node* dst, bool bind){
    auto src_inner_type_data = src->getData("inner_type");
    auto src_outer_type_data = src->getData("outer_type");
    auto src_type_data = src->getData("type");

    auto dst_inner_type_data = dst->getData("inner_type");
    auto dst_outer_type_data = dst->getData("outer_type");
    auto dst_type_data = dst->getData("type");

    //Got fully described data
    if(src_inner_type_data && src_outer_type_data && src_type_data){
        if(dst_inner_type_data){
            src_inner_type_data->linkData(dst_inner_type_data, bind);
        }
        if(dst_outer_type_data){
            src_outer_type_data->linkData(dst_outer_type_data, bind);
        }
        if(dst_type_data){
            src_type_data->linkData(dst_type_data, bind);
        }
    }else if(src_type_data){
        if(dst_inner_type_data){
            src_type_data->linkData(dst_inner_type_data, bind);
        }else if(dst_type_data){
            src_type_data->linkData(dst_type_data, bind);
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

bool TypeKey::CompareTypes(Node* node_1, Node* node_2){
    if(node_1 && node_2){
        const auto number_types = GetNumberTypes();

        //Variadic Parameters can always use Anything
        if(node_2->getNodeKind() == NODE_KIND::VARIADIC_PARAMETER){
           return true;
        }

        auto type_1 = node_1->getDataValue("type").toString();
        auto type_2 = node_2->getDataValue("type").toString();

        if(type_1 == type_2 && type_1.size()){
            //Allow Exact matches
            return true;
        }

        if(number_types.contains(type_1) && number_types.contains(type_2)){
            //Allow matches of numbers
            return true;
        }

        if(type_2 == ""){
            return true;
        }

        auto outer_type_data_1 = node_1->getData("outer_type");
        auto outer_type_data_2 = node_2->getData("outer_type");

        auto inner_type_data_1 = node_1->getData("inner_type");
        auto inner_type_data_2 = node_2->getData("inner_type");


        if(outer_type_data_1 && outer_type_data_2 && inner_type_data_1 && inner_type_data_2){
            auto outer_type_1 = outer_type_data_1->getValue().toString();
            auto outer_type_2 = outer_type_data_2->getValue().toString();
            auto outer_types_match = outer_type_1 == outer_type_2;
            auto outer_types_empty = outer_types_match && outer_type_1.isEmpty();

            auto inner_type_1 = inner_type_data_1->getValue().toString();
            auto inner_type_2 = inner_type_data_2->getValue().toString();
            auto inner_types_match = inner_type_1 == inner_type_2;
            auto inner_types_empty = inner_types_match && inner_type_1.isEmpty();


            if(outer_types_match){
                if(inner_types_match && !inner_types_empty){
                    return true;
                }else if(inner_type_2 == ""){
                    return true;
                }
            }
        }
    }
    return false;
}