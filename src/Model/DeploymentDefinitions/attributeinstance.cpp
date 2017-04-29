#include "attributeinstance.h"

AttributeInstance::AttributeInstance():Node(Node::NK_ATTRIBUTE_INSTANCE)
{
    setNodeType(NT_INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    //Locked Label
    updateDefaultData("label", QVariant::String, true);
    updateDefaultData("type", QVariant::String, true);
    updateDefaultData("sortOrder", QVariant::Int, true);
    updateDefaultData("value", QVariant::String);
}

bool AttributeInstance::canAdoptChild(Node*)
{
    return false;
}

bool AttributeInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NK_ATTRIBUTE){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
