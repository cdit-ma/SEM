#include "outeventport.h"
#include "../entityfactory.h"

OutEventPort::OutEventPort(EntityFactory* factory) : EventPort(factory, NODE_KIND::OUTEVENTPORT, "OutEventPort"){
	auto node_kind = NODE_KIND::OUTEVENTPORT;
	QString kind_string = "OutEventPort";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPort();});
};

OutEventPort::OutEventPort():EventPort(NODE_KIND::OUTEVENTPORT)
{
	
}

bool OutEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

bool OutEventPort::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPort::canAcceptEdge(edgeKind, dst);
}

