#include "ineventportinstance.h"

InEventPortInstance::InEventPortInstance():EventPortAssembly(NODE_KIND::INEVENTPORT_INSTANCE)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_QOS);
    removeEdgeKind(Edge::EC_AGGREGATE);

    
}

bool InEventPortInstance::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NODE_KIND::INEVENTPORT)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}
