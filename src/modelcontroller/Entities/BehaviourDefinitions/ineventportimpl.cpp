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
    addImplsDefinitionKind(NODE_KIND::INEVENTPORT);

    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
}


bool InEventPortImpl::canAdoptChild(Node *child)
{
    auto child_node_kind = child->getNodeKind();
    
    switch(child_node_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(getChildrenOfKind(child_node_kind, 0).size() > 0){
                return false;
            }
            break;
        default:
            break;
        }
    }
    return Node::canAdoptChild(child);
}
