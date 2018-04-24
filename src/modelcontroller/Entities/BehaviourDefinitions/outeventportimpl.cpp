#include "outeventportimpl.h"

#include "../../edgekinds.h"


const NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT_IMPL;
const QString kind_string = "OutEventPortImpl";


OutEventPortImpl::OutEventPortImpl(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortImpl();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

OutEventPortImpl::OutEventPortImpl():Node(node_kind){
    addImplsDefinitionKind(NODE_KIND::OUTEVENTPORT);
    
    //Allow links from within things like InEventPortImpls back to the
    SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
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

bool OutEventPortImpl::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::OUTEVENTPORT){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
