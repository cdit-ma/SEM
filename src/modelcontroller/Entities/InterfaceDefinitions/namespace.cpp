#include "namespace.h"
#include "../data.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::NAMESPACE;
const QString kind_string = "Namespace";

Namespace::Namespace(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
    RegisterDefaultData(factory, node_kind, "namespace", QVariant::String, true);
    
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Namespace();});
};

Namespace::Namespace(): Node(node_kind)
{
}


bool Namespace::canAdoptChild(Node* child)
{
    //Dissallow SHARED_DATATYPES
    switch(child->getNodeKind()){
    case NODE_KIND::SHARED_DATATYPES:
        return false;
    default:
        break;
    }

    auto parent_node = getParentNode();

    if(parent_node){
        return parent_node->canAdoptChild(child);
    }
    return false;
}

bool Namespace::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}

void Namespace::BindNamespace(Node* parent, Node* child, bool bind){
    auto src_namespace_data = parent->getData("namespace");
    auto dst_namespace_data = child->getData("namespace");
    if(src_namespace_data && dst_namespace_data){
        src_namespace_data->linkData(dst_namespace_data, bind);
    }
}

void Namespace::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace"){
        auto label_data = getData("label");
        auto namespace_data = getData("namespace");

        if(label_data && namespace_data){
            label_data->linkData(namespace_data, true);
        }
    }
}




void Namespace::childAdded(Node* child){
    Node::childAdded(child);
    Namespace::BindNamespace(this, child, true);
}

void Namespace::childRemoved(Node* child){
    Node::childRemoved(child);
    Namespace::BindNamespace(this, child, false);
}
