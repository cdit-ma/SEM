#include "outeventportdelegate.h"
//#include "../entityfactory.h"

OutEventPortDelegate::OutEventPortDelegate(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::OUTEVENTPORT_DELEGATE, "OutEventPortDelegate"){
	auto node_kind = NODE_KIND::OUTEVENTPORT_DELEGATE;
	QString kind_string = "OutEventPortDelegate";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortDelegate();});
};

OutEventPortDelegate::OutEventPortDelegate():EventPortAssembly(NODE_KIND::OUTEVENTPORT_DELEGATE)
{
    removeEdgeKind(EDGE_KIND::DEFINITION);
    removeNodeType(NODE_TYPE::DEFINITION);
}

bool OutEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool OutEventPortDelegate::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

