#include "outeventport.h"


OutEventPort::OutEventPort(EntityFactory* factory) : EventPort(factory, NODE_KIND::OUTEVENTPORT, "OutEventPort"){
	auto node_kind = NODE_KIND::OUTEVENTPORT;
	QString kind_string = "OutEventPort";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPort();});
};

OutEventPort::OutEventPort():EventPort(NODE_KIND::OUTEVENTPORT)
{
	addImplKind(NODE_KIND::OUTEVENTPORT_IMPL);
	addInstanceKind(NODE_KIND::OUTEVENTPORT_INSTANCE);
}