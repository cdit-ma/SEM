#include "ineventportimpl.h"
#include "../nodekinds.h"

InEventPortImpl::InEventPortImpl():EventPortImpl(NODE_KIND::INEVENTPORT_IMPL){
    setWorkflowProducer(true);
    setWorkflowReciever(false);

    updateDefaultData("type", QVariant::String, true);
}

bool InEventPortImpl::canAdoptChild(Node *child)
{
    return EventPortImpl::canAdoptChild(child);
}

bool InEventPortImpl::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(!dst->getImplementations().isEmpty()){
            return false;
        }
        if(dst->getNodeKind() != NODE_KIND::INEVENTPORT){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return EventPortImpl::canAcceptEdge(edgeKind, dst);
}

