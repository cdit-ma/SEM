#include "namespacekey.h"
#include "../data.h"
#include "../node.h"
#include <QDebug>

NamespaceKey::NamespaceKey(): Key("namespace", QVariant::String){

}

QVariant NamespaceKey::validateDataChange(Data* data, QVariant data_value){
    
    QString desired_namespace = data_value.toString();
    QString new_namespace = desired_namespace;

    auto entity = data->getParent();
    qCritical() << data->toString() << "TRYING TO SET: " << data_value;

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


    auto node_namespace = node->getData(this);
    auto parent_namespace = parent_node->getData(this);

    auto parent_namespace_value = parent_namespace ? parent_namespace->getValue().toString() : "";

    if(parent_kind == NODE_KIND::NAMESPACE){
        new_namespace = parent_namespace_value;
    }else{
        new_namespace.clear();
    }

    if(node_kind == NODE_KIND::NAMESPACE){
        if(new_namespace.size()){
            new_namespace += "::";
        }
        new_namespace += node->getData("label")->getValue().toString();
    }

    return new_namespace;
}
