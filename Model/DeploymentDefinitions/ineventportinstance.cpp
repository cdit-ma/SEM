#include "ineventportinstance.h"

InEventPortInstance::InEventPortInstance():EventPortAssembly(NK_INEVENTPORT_INSTANCE)
{
    setNodeType(NT_INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_QOS);
}

bool InEventPortInstance::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortInstance::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NK_INEVENTPORT)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}
