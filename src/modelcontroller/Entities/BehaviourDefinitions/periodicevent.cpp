#include "periodicevent.h"



PeriodicEvent::PeriodicEvent(EntityFactory* factory) : Node(factory, NODE_KIND::PERIODICEVENT, "PeriodicEvent"){
	auto node_kind = NODE_KIND::PERIODICEVENT;
	QString kind_string = "PeriodicEvent";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new PeriodicEvent();});

    RegisterDefaultData(factory, node_kind, "frequency", QVariant::Double, false, 1);
};

PeriodicEvent::PeriodicEvent():Node(NODE_KIND::PERIODICEVENT){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    //setNodeType(NODE_TYPE::DEFINITION);
    //setNodeType(NODE_TYPE::INSTANCE);
    //setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    //setDefinitionKind(NODE_KIND::PERIODICEVENT);
    //setInstanceKind(NODE_KIND::PERIODICEVENT);
}

bool PeriodicEvent::canAdoptChild(Node* child)
{
    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool PeriodicEvent::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
