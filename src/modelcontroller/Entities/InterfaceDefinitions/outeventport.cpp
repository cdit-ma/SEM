#include "outeventport.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT;
const static QString kind_string = "OutEventPort";

void OutEventPort::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OutEventPort(broker, is_temp_node);
    });
}

OutEventPort::OutEventPort(EntityFactoryBroker& broker, bool is_temp) : EventPort(broker, node_kind, is_temp){
    //Setup State
    addImplKind(NODE_KIND::OUTEVENTPORT_IMPL);
	addInstanceKind(NODE_KIND::OUTEVENTPORT_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    broker.AttachData(this, "row", QVariant::Int, 2, true);
    broker.ProtectData(this, "index", false);
}