#include "outeventportimpl.h"
#include "../nodekinds.h"
#include <QDebug>

OutEventPortImpl::OutEventPortImpl():EventPortImpl(NODE_KIND::OUTEVENTPORT_IMPL){
    setWorkflowProducer(true);
    setWorkflowReciever(true);
    
    updateDefaultData("type", QVariant::String, true);
}

bool OutEventPortImpl::canAdoptChild(Node *child)
{
    return EventPortImpl::canAdoptChild(child);
}

bool OutEventPortImpl::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::OUTEVENTPORT){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return EventPortImpl::canAcceptEdge(edgeKind, dst);
}
