#include "outeventportinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

auto node_kind = NODE_KIND::OUTEVENTPORT_INSTANCE;
QString kind_string = "OutEventPortInstance";

void OutEventPortInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OutEventPortInstance(broker, is_temp_node);
    });
}

OutEventPortInstance::OutEventPortInstance(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::OUTEVENTPORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::UNPROTECTED, 2);

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "TAO"});

    connect(data_middleware, &Data::dataChanged, this, &OutEventPortInstance::MiddlewareUpdated);
    MiddlewareUpdated();
}


