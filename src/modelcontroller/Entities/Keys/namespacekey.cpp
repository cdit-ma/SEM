#include "namespacekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>


NamespaceKey::NamespaceKey(EntityFactoryBroker& broker): Key(broker, KeyName::Namespace, QVariant::String){

}

QString NamespaceKey::CombineNamespaces(QString namespace_1, QString namespace_2){
    QString combined_namespace = namespace_1;
    
    if(combined_namespace.size()){
        combined_namespace += "::";
    }
    combined_namespace += namespace_2;
    return combined_namespace;
}

QVariant NamespaceKey::validateDataChange(Data* data, QVariant data_value){
    
    QString desired_namespace = data_value.toString();
    QString new_namespace = desired_namespace;

    auto entity = data->getParent();

    Node* node = 0;
    Node* parent_node = 0;

    if(entity && entity->isNode()){
        node = (Node*) entity;
        parent_node = node->getParentNode();
    }
    
    if(!node || !parent_node){
        return new_namespace;
    }

    auto node_kind = node->getNodeKind();
    auto parent_kind = parent_node->getNodeKind();
    auto parent_namespace = parent_node->getData(this);

    auto parent_namespace_value = parent_namespace ? parent_namespace->getValue().toString() : "";

    if(parent_kind == NODE_KIND::NAMESPACE){
        new_namespace = parent_namespace_value;
    }else{
        new_namespace.clear();
    }

    if(node_kind == NODE_KIND::NAMESPACE){
        new_namespace = CombineNamespaces(parent_namespace_value, node->getData(KeyName::Label)->getValue().toString());
    }

    return new_namespace;
}
