#include "publisherportinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

auto node_kind = NODE_KIND::PORT_PUBLISHER_INST;
QString kind_string = "Publisher Port Instance";

void PublisherPortInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PublisherPortInst(broker, is_temp_node);
    });
}

PublisherPortInst::PublisherPortInst(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, true){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PORT_PUBLISHER);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::UNPROTECTED, 2);
    broker.AttachData(this, "column", QVariant::Int, ProtectedState::UNPROTECTED, 0);

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "QPID"});

    connect(data_middleware, &Data::dataChanged, this, &PublisherPortInst::MiddlewareUpdated);
    MiddlewareUpdated();
}


