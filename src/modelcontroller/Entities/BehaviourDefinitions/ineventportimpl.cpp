#include "ineventportimpl.h"

#include "../../edgekinds.h"

const NODE_KIND node_kind = NODE_KIND::INEVENTPORT_IMPL;
const QString kind_string = "InEventPortImpl";


InEventPortImpl::InEventPortImpl(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPortImpl();});
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

InEventPortImpl::InEventPortImpl() : Node(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setDefinitionKind(NODE_KIND::INEVENTPORT);
}


bool InEventPortImpl::canAdoptChild(Node *child)
{
    auto child_node_kind = child->getNodeKind();
    
    switch(child_node_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(getChildrenOfKind(child_node_kind, 0).size() >= 1){
                return false;
            }
            break;
        default:
            if(!ContainerNode::canAdoptChild(child)){
                return false;
            }
        }
    }
    return Node::canAdoptChild(child);
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

    return Node::canAcceptEdge(edgeKind, dst);
}

