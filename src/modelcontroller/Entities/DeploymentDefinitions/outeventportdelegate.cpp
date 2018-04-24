#include "outeventportdelegate.h"

#include "../../edgekinds.h"

OutEventPortDelegate::OutEventPortDelegate(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::OUTEVENTPORT_DELEGATE, "OutEventPortDelegate"){
	auto node_kind = NODE_KIND::OUTEVENTPORT_DELEGATE;
	QString kind_string = "OutEventPortDelegate";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortDelegate();});
};

OutEventPortDelegate::OutEventPortDelegate():EventPortAssembly(NODE_KIND::OUTEVENTPORT_DELEGATE)
{
}

bool OutEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}