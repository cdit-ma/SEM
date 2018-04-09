#include "setter.h"


const NODE_KIND node_kind = NODE_KIND::SETTER;
const QString kind_string = "Setter";


Setter::Setter(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Setter();});


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "operator", QVariant::String, false, "=");
    QList<QVariant> operator_types = {"=", "+=", "-=", "*=", "/="};

    RegisterValidDataValues(factory, node_kind, "operator", QVariant::String, operator_types);
};

Setter::Setter() : Node(node_kind)
{
}

bool Setter::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::INPUT_PARAMETER:
    case NODE_KIND::VARIADIC_PARAMETER:
        break;
    default:
        return false;
    }

    if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool Setter::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return false;
}
