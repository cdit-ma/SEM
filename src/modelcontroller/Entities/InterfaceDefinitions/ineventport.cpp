#include "ineventport.h"


InEventPort::InEventPort(EntityFactory* factory) : EventPort(factory, NODE_KIND::INEVENTPORT, "InEventPort"){
	auto node_kind = NODE_KIND::INEVENTPORT;
	QString kind_string = "InEventPort";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InEventPort();});
};

InEventPort::InEventPort():EventPort(NODE_KIND::INEVENTPORT)
{
	addImplKind(NODE_KIND::INEVENTPORT_IMPL);
	addInstanceKind(NODE_KIND::INEVENTPORT_INSTANCE);
}