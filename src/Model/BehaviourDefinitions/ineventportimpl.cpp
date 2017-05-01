#include "ineventportimpl.h"
#include "../nodekinds.h"

#include "../entityfactory.h"

InEventPortImpl::InEventPortImpl(EntityFactory* factory) : EventPortImpl(factory, NODE_KIND::INEVENTPORT_IMPL, "InEventPortImpl"){
	auto node_kind = NODE_KIND::INEVENTPORT_IMPL;
	QString kind_string = "InEventPortImpl";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPortImpl();});
};

InEventPortImpl::InEventPortImpl():EventPortImpl(NODE_KIND::INEVENTPORT_IMPL){
    setWorkflowProducer(true);
    setWorkflowReciever(false);
}

bool InEventPortImpl::canAdoptChild(Node *child)
{
    return EventPortImpl::canAdoptChild(child);
}

bool InEventPortImpl::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
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

