#include "ineventportdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::INEVENTPORT_DELEGATE;
const static QString kind_string = "InEventPortDelegate";

void InEventPortDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new InEventPortDelegate(broker, is_temp_node);
    });
}

InEventPortDelegate::InEventPortDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp){
}