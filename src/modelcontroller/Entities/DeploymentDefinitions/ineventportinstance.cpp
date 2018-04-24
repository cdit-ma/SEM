#include "ineventportinstance.h"

#include "../../edgekinds.h"

InEventPortInstance::InEventPortInstance(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::INEVENTPORT_INSTANCE, "InEventPortInstance"){
	auto node_kind = NODE_KIND::INEVENTPORT_INSTANCE;
	QString kind_string = "InEventPortInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPortInstance();});
};

InEventPortInstance::InEventPortInstance():EventPortAssembly(NODE_KIND::INEVENTPORT_INSTANCE)
{
    addInstancesDefinitionKind(NODE_KIND::INEVENTPORT);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
}

bool InEventPortInstance::canAdoptChild(Node*)
{
    return false;
}
