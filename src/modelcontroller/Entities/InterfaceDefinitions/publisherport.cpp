#include "publisherport.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_PUBLISHER;
const QString kind_string = "Publisher Port";

void PublisherPort::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PublisherPort(broker, is_temp_node);
    });
}

PublisherPort::PublisherPort(EntityFactoryBroker& broker, bool is_temp) : EventPort(broker, node_kind, is_temp, true){
    //Setup State
    addImplKind(NODE_KIND::PORT_PUBLISHER_IMPL);
	addInstanceKind(NODE_KIND::PORT_PUBLISHER_INST);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 2);
}