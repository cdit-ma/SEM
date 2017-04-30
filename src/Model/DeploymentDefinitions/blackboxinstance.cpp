#include "blackboxinstance.h"

BlackBoxInstance::BlackBoxInstance():Node(NODE_KIND::BLACKBOX_INSTANCE)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);

    updateDefaultData("label", QVariant::String, false);
    updateDefaultData("type", QVariant::String, true);
}

bool BlackBoxInstance::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Instances
    switch(child->getNodeKind()){
    case NODE_KIND::INEVENTPORT_INSTANCE:
    case NODE_KIND::OUTEVENTPORT_INSTANCE:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool BlackBoxInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::BLACKBOX){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
