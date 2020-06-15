#include "requestportdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_REQUEST_DELEGATE;
const QString kind_string = "Request Port Delegate";

void RequestPortDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new RequestPortDelegate(broker, is_temp_node);
    });
}

RequestPortDelegate::RequestPortDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, false){

}