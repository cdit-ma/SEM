#include "outeventportimpl.h"
#include <QDebug>

OutEventPortImpl::OutEventPortImpl():EventPortImpl(NK_OUTEVENTPORT_IMPL){
    setWorkflowProducer(true);
    setWorkflowReciever(true);
}

bool OutEventPortImpl::canAdoptChild(Node *child)
{
    return EventPortImpl::canAdoptChild(child);
}

bool OutEventPortImpl::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NK_OUTEVENTPORT){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return EventPortImpl::canAcceptEdge(edgeKind, dst);
}
