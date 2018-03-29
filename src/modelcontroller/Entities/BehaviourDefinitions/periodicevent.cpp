#include "periodicevent.h"



PeriodicEvent::PeriodicEvent(EntityFactory* factory) : ContainerNode(factory, NODE_KIND::PERIODICEVENT, "PeriodicEvent"){
	auto node_kind = NODE_KIND::PERIODICEVENT;
	QString kind_string = "PeriodicEvent";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new PeriodicEvent();});

    RegisterDefaultData(factory, node_kind, "frequency", QVariant::Double, false, 1);
};

PeriodicEvent::PeriodicEvent():ContainerNode(NODE_KIND::PERIODICEVENT){

}

bool PeriodicEvent::canAdoptChild(Node* child)
{
    return ContainerNode::canAdoptChild(child);
}

bool PeriodicEvent::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return ContainerNode::canAcceptEdge(edgeKind, dst);
}
