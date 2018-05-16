#include "ineventport.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::INEVENTPORT;
const static QString kind_string = "InEventPort";

void InEventPort::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new InEventPort(broker, is_temp_node);
    });
}

InEventPort::InEventPort(EntityFactoryBroker& broker, bool is_temp) : EventPort(broker, node_kind, is_temp){
    //Setup State
    addImplKind(NODE_KIND::INEVENTPORT_IMPL);
	addInstanceKind(NODE_KIND::INEVENTPORT_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    broker.AttachData(this, "row", QVariant::Int, 0, true);
    broker.ProtectData(this, "index", false);
}