#include "outeventportinstance.h"
#include <QDebug>

OutEventPortInstance::OutEventPortInstance(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::OUTEVENTPORT_INSTANCE, "OutEventPortInstance"){
	auto node_kind = NODE_KIND::OUTEVENTPORT_INSTANCE;
	QString kind_string = "OutEventPortInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortInstance();});
};

OutEventPortInstance::OutEventPortInstance():EventPortAssembly(NODE_KIND::OUTEVENTPORT_INSTANCE)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_QOS);
    removeEdgeKind(Edge::EC_AGGREGATE);
}

bool OutEventPortInstance::canAdoptChild(Node*)
{
    return false;
}

bool OutEventPortInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::OUTEVENTPORT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}
