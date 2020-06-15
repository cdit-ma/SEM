#include "pubsubportdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_PUBSUB_DELEGATE;
const QString kind_string = "PubSub Port Delegate";

void PubSubPortDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PubSubPortDelegate(broker, is_temp_node);
    });
}

PubSubPortDelegate::PubSubPortDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, true){

}