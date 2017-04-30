#include "componentassembly.h"

ComponentAssembly::ComponentAssembly():Node(NODE_KIND::COMPONENT_ASSEMBLY)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setAcceptsEdgeKind(Edge::EC_QOS);

    updateDefaultData("replicate_count", QVariant::Int, false, 1);
}

bool ComponentAssembly::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Instances
    switch(child->getNodeKind()){
    case NODE_KIND::BLACKBOX_INSTANCE:
    case NODE_KIND::COMPONENT_ASSEMBLY:
    case NODE_KIND::COMPONENT_INSTANCE:
    case NODE_KIND::INEVENTPORT_DELEGATE:
    case NODE_KIND::OUTEVENTPORT_DELEGATE:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool ComponentAssembly::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    //Can only have one Deployed Node
    if(edgeKind == Edge::EC_DEPLOYMENT){
        if(!getEdges(0, edgeKind).empty()){
            return false;
        }
    }

    return Node::canAcceptEdge(edgeKind, dst);
}
