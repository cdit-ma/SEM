#include "typekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>
#include "namespacekey.h"

TypeKey::TypeKey(EntityFactoryBroker& broker): Key(broker, KeyName::Type, QVariant::String){
    //Combine Namespace and Type
    combine_namespace_kinds = {NODE_KIND::AGGREGATE, NODE_KIND::COMPONENT, NODE_KIND::ENUM, NODE_KIND::CLASS, NODE_KIND::SERVER_INTERFACE};
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


const QString type_String("String");
const QString type_Boolean("Boolean");
const QString type_Integer("Integer");
const QString type_Double("Double");
const QString type_Float("Float");
const QString type_Character("Character");

const QString cpptype_Boolean("bool");
const QString cpptype_Integer("int");
const QString cpptype_Double("double");
const QString cpptype_Float("float");
const QString cpptype_Character("char");



const QSet<QString>&  TypeKey::GetPrimitiveTypes(){
    const static QSet<QString> primitive_types({type_String, type_Boolean, type_Integer, type_Double, type_Float, type_Character});
    return primitive_types;
}

const QSet<QString>&  TypeKey::GetNumberTypes(){
    const static QSet<QString> number_types({type_Integer, type_Double, type_Float, type_Character});
    return number_types;
}

QString TypeKey::GetCPPPrimitiveType(const QString& type){
    const static QHash<QString, QString> cpp_types({
            {type_Boolean, cpptype_Boolean},
            {type_Double, cpptype_Double},
            {type_Float, cpptype_Float},
            {type_Integer, cpptype_Integer},
            {type_Character, cpptype_Character},
        });

    return cpp_types.value(type, type);
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
        auto namespace_data = entity->getData(KeyName::Namespace);
        auto label_data = entity->getData(KeyName::Label);

        auto namespace_value = namespace_data ? namespace_data->getValue().toString() : "";
        auto label_value = label_data ? label_data->getValue().toString() : "";

        new_type = NamespaceKey::CombineNamespaces(namespace_value, label_value);
    }else{
        auto inner_type_data = entity->getData(KeyName::InnerType);
        auto outer_type_data = entity->getData(KeyName::OuterType);

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


void TypeKey::BindNamespaceAndLabelToType(Node* node, bool bind){
    auto namespace_data = node->getData(KeyName::Namespace);
    auto label_data = node->getData(KeyName::Label);
    auto type_data = node->getData(KeyName::Type);
    if(namespace_data && label_data && type_data){
        namespace_data->linkData(type_data, bind);
        label_data->linkData(type_data, bind);
    }
}

bool TypeKey::BindTypes(Node* src, Node* dst, bool bind_outer, bool bind){
    //Try bind Inner to Inner
    auto success = Data::LinkData(src, KeyName::InnerType, dst, KeyName::InnerType, bind);
    if(!success){
        //Try Type to Inner
        success = Data::LinkData(src, KeyName::Type, dst, KeyName::InnerType, bind);
    }
    if(success){
        success = Data::LinkData(src, KeyName::OuterType, dst, KeyName::OuterType, bind);
    }
    return success;
}

bool TypeKey::CompareTypes(Node* node_1, Node* node_2){
    if(node_1 && node_2){
        const auto number_types = GetNumberTypes();

        //Variadic Parameters can always use Anything
        if(node_2->getNodeKind() == NODE_KIND::VARIADIC_PARAMETER){
           return true;
        }

        auto type_1 = node_1->getDataValue(KeyName::Type).toString();
        auto type_2 = node_2->getDataValue(KeyName::Type).toString();

        if(type_1 == type_2 && type_1.size()){
            //Allow Exact matches
            return true;
        }

        if(number_types.contains(type_1) && number_types.contains(type_2)){
            //Allow matches of numbers
            return true;
        }

        if(type_2.isEmpty()){
            return true;
        }

        auto outer_type_data_1 = node_1->getData(KeyName::OuterType);
        auto outer_type_data_2 = node_2->getData(KeyName::OuterType);

        auto inner_type_data_1 = node_1->getData(KeyName::InnerType);
        auto inner_type_data_2 = node_2->getData(KeyName::InnerType);


        if(outer_type_data_1 && outer_type_data_2 && inner_type_data_1 && inner_type_data_2){
            auto outer_type_1 = outer_type_data_1->getValue().toString();
            auto outer_type_2 = outer_type_data_2->getValue().toString();
            auto outer_types_match = outer_type_1 == outer_type_2;

            auto inner_type_1 = inner_type_data_1->getValue().toString();
            auto inner_type_2 = inner_type_data_2->getValue().toString();
            auto inner_types_match = inner_type_1 == inner_type_2;
            auto inner_types_empty = inner_types_match && inner_type_1.isEmpty();


            if(outer_types_match){
                if(inner_types_match && !inner_types_empty){
                    return true;
                }else if(inner_type_2.isEmpty()){
                    return true;
                }
            }
        }
    }
    return false;
}