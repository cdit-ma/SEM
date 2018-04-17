#include "ineventportinstance.h"

#include "../../edgekinds.h"

InEventPortInstance::InEventPortInstance(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::INEVENTPORT_INSTANCE, "InEventPortInstance"){
	auto node_kind = NODE_KIND::INEVENTPORT_INSTANCE;
	QString kind_string = "InEventPortInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPortInstance();});
};

InEventPortInstance::InEventPortInstance():EventPortAssembly(NODE_KIND::INEVENTPORT_INSTANCE)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::QOS);
    removeEdgeKind(EDGE_KIND::AGGREGATE);

    setDefinitionKind(NODE_KIND::INEVENTPORT);
    
}

bool InEventPortInstance::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NODE_KIND::INEVENTPORT)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}
