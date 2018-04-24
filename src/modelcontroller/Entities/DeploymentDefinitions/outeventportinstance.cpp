#include "outeventportinstance.h"

#include "../../edgekinds.h"
#include <QDebug>

OutEventPortInstance::OutEventPortInstance(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::OUTEVENTPORT_INSTANCE, "OutEventPortInstance"){
	auto node_kind = NODE_KIND::OUTEVENTPORT_INSTANCE;
	QString kind_string = "OutEventPortInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortInstance();});
};

OutEventPortInstance::OutEventPortInstance():EventPortAssembly(NODE_KIND::OUTEVENTPORT_INSTANCE)
{
    addInstancesDefinitionKind(NODE_KIND::OUTEVENTPORT);

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);
}

bool OutEventPortInstance::canAdoptChild(Node*)
{
    return false;
}