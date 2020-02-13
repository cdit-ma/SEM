#include "subscriberportinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::PORT_SUBSCRIBER_INST;
const static QString kind_string = "Subscriber Port Instance";

void SubscriberPortInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new SubscriberPortInst(broker, is_temp_node);
    });
}

SubscriberPortInst::SubscriberPortInst(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, true){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PORT_SUBSCRIBER);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::UNPROTECTED, 0);
    broker.AttachData(this, "column", QVariant::Int, ProtectedState::UNPROTECTED, 0);

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "QPID"});

    connect(data_middleware, &Data::dataChanged, this, &SubscriberPortInst::MiddlewareUpdated);
    MiddlewareUpdated();
}