#include "subscriberportdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::PORT_SUBSCRIBER_DELEGATE;
const static QString kind_string = "Subscriber Port Delegate";

void SubscriberPortDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new SubscriberPortDelegate(broker, is_temp_node);
    });
}

SubscriberPortDelegate::SubscriberPortDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp){
}