#include "periodicevent.h"



PeriodicEvent::PeriodicEvent(EntityFactory* factory) : Node(factory, NODE_KIND::PERIODICEVENT, "PeriodicEvent"){
	auto node_kind = NODE_KIND::PERIODICEVENT;
	QString kind_string = "PeriodicEvent";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new PeriodicEvent();});
};

PeriodicEvent::PeriodicEvent():Node(NODE_KIND::PERIODICEVENT){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

}

#include <QDebug>

bool PeriodicEvent::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::ATTRIBUTE_INSTANCE:{
            //Only allow one AttributeInstance
            if(getChildrenOfKind(child_kind, 0).size() > 0){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

bool PeriodicEvent::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::PERIODICEVENT:{
                if(getViewAspect() == VIEW_ASPECT::ASSEMBLIES){
                    if(dst->getViewAspect() != VIEW_ASPECT::BEHAVIOUR) {
                        return false;
                    }
                }else{
                    return false;
                }
                break;
            }
            default:
                return false;
        }
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
