#include "ineventportinstance.h"
#include "../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::INEVENTPORT_INSTANCE;
const static QString kind_string = "InEventPortInstance";

void InEventPortInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new InEventPortInstance(factory, is_temp_node);
    });
}

InEventPortInstance::InEventPortInstance(EntityFactory& factory, bool is_temp) : EventPortAssembly(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::INEVENTPORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);

    auto data_middleware = factory.AttachData(this, "middleware", QVariant::String, "ZMQ", true);
    connect(data_middleware, &Data::dataChanged, this, &InEventPortInstance::updateQOSEdge);
    
    updateQOSEdge();
}

void InEventPortInstance::updateQOSEdge(){
    auto middleware_value = getDataValue("middleware").toString();
    QSet<QString> qos_middlewares = {"RTI", "OSPL"};
    
    //Check for QOSable Middlewares
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE, qos_middlewares.contains(middleware_value));
}