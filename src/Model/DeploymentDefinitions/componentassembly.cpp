#include "componentassembly.h"

ComponentAssembly::ComponentAssembly():Node(NK_COMPONENT_ASSEMBLY)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setAcceptsEdgeKind(Edge::EC_QOS);

    updateDefaultData("replicate_count", QVariant::Int, false, 1);
}

bool ComponentAssembly::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Instances
    switch(child->getNodeKind()){
    case NK_BLACKBOX_INSTANCE:
    case NK_COMPONENT_ASSEMBLY:
    case NK_COMPONENT_INSTANCE:
    case NK_INEVENTPORT_DELEGATE:
    case NK_OUTEVENTPORT_DELEGATE:
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
