#include "ineventportdelegate.h"

#include "../../edgekinds.h"


InEventPortDelegate::InEventPortDelegate(EntityFactory* factory) : EventPortAssembly(factory, NODE_KIND::INEVENTPORT_DELEGATE, "InEventPortDelegate"){
	auto node_kind = NODE_KIND::INEVENTPORT_DELEGATE;
	QString kind_string = "InEventPortDelegate";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPortDelegate();});
};

InEventPortDelegate::InEventPortDelegate():EventPortAssembly(NODE_KIND::INEVENTPORT_DELEGATE)
{
    
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}
