#include "namespace.h"

const NODE_KIND node_kind = NODE_KIND::NAMESPACE;
const QString kind_string = "Namespace";

Namespace::Namespace(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
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
