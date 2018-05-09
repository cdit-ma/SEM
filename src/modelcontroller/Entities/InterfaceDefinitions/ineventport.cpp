#include "ineventport.h"
#include "../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::INEVENTPORT;
const static QString kind_string = "InEventPort";

void InEventPort::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new InEventPort(factory, is_temp_node);
    });
}

InEventPort::InEventPort(EntityFactory& factory, bool is_temp) : EventPort(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addImplKind(NODE_KIND::INEVENTPORT_IMPL);
	addInstanceKind(NODE_KIND::INEVENTPORT_INSTANCE);
}