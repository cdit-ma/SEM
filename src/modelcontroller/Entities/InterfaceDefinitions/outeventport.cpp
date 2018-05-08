#include "outeventport.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT;
const QString kind_string = "OutEventPort";

void OutEventPort::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new OutEventPort(factory, is_temp_node);
    });
}

OutEventPort::OutEventPort(EntityFactory& factory, bool is_temp) : EventPort(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addImplKind(NODE_KIND::OUTEVENTPORT_IMPL);
	addInstanceKind(NODE_KIND::OUTEVENTPORT_INSTANCE);
}