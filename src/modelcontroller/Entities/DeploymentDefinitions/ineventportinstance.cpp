#include "ineventportinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::INEVENTPORT_INSTANCE;
const static QString kind_string = "InEventPortInstance";

void InEventPortInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new InEventPortInstance(broker, is_temp_node);
    });
}

InEventPortInstance::InEventPortInstance(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::INEVENTPORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::UNPROTECTED, 0);

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "TAO"});

    connect(data_middleware, &Data::dataChanged, this, &InEventPortInstance::MiddlewareUpdated);
    MiddlewareUpdated();
}