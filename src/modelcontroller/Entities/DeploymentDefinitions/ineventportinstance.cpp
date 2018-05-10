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
    
    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, "ZMQ", true);
    connect(data_middleware, &Data::dataChanged, this, &InEventPortInstance::updateQOSEdge);
    
    updateQOSEdge();
}

void InEventPortInstance::updateQOSEdge(){
    auto middleware_value = getDataValue("middleware").toString();
    QSet<QString> qos_middlewares = {"RTI", "OSPL"};
    
    //Check for QOSable Middlewares
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE, qos_middlewares.contains(middleware_value));
}