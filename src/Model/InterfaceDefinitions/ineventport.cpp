#include "ineventport.h"
#include "../entityfactory.h"

InEventPort::InEventPort(EntityFactory* factory) : EventPort(factory, NODE_KIND::INEVENTPORT, "InEventPort"){
	auto node_kind = NODE_KIND::INEVENTPORT;
	QString kind_string = "InEventPort";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPort();});
};

InEventPort::InEventPort():EventPort(NODE_KIND::INEVENTPORT)
{
	setImplKind(NODE_KIND::INEVENTPORT_IMPL);
	setInstanceKind(NODE_KIND::INEVENTPORT_INSTANCE);
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

bool InEventPort::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return EventPort::canAcceptEdge(edgeKind, dst);
}
