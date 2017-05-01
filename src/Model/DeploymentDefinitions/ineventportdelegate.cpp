#include "ineventportdelegate.h"

#include "../entityfactory.h"

InEventPortDelegate::InEventPortDelegate(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::INEVENTPORT_DELEGATE, "InEventPortDelegate"){
	auto node_kind = NODE_KIND::INEVENTPORT_DELEGATE;
	QString kind_string = "InEventPortDelegate";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPortDelegate();});
};

InEventPortDelegate::InEventPortDelegate():EventPortAssembly(NODE_KIND::INEVENTPORT_DELEGATE)
{
    removeEdgeKind(Edge::EC_DEFINITION);
    removeNodeType(NODE_TYPE::DEFINITION);
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortDelegate::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

