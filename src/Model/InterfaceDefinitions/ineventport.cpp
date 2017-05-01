#include "ineventport.h"
#include "../entityfactory.h"

InEventPort::InEventPort(EntityFactory* factory) : EventPort(factory, NODE_KIND::INEVENTPORT, "InEventPort"){
	auto node_kind = NODE_KIND::INEVENTPORT;
	QString kind_string = "InEventPort";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPort();});
};

InEventPort::InEventPort():EventPort(NODE_KIND::INEVENTPORT)
{
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

bool InEventPort::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPort::canAcceptEdge(edgeKind, dst);
}
