#include "outeventportimpl.h"

#include "../../edgekinds.h"


const NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT_IMPL;
const QString kind_string = "OutEventPortImpl";


OutEventPortImpl::OutEventPortImpl(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortImpl();});
};

OutEventPortImpl::OutEventPortImpl():Node(node_kind){
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setDefinitionKind(NODE_KIND::OUTEVENTPORT);
}

bool OutEventPortImpl::canAdoptChild(Node *child)
{
    //Can Only accept 1 child.
    if(hasChildren()){
        return false;
    }

    //Can only adopt AggregateInstances
    if(child->getNodeKind() != NODE_KIND::AGGREGATE_INSTANCE){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool OutEventPortImpl::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
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

    return Node::canAcceptEdge(edgeKind, dst);
}
