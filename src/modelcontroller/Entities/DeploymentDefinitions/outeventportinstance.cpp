#include "outeventportinstance.h"
#include "../../entityfactory.h"

auto node_kind = NODE_KIND::OUTEVENTPORT_INSTANCE;
QString kind_string = "OutEventPortInstance";

void OutEventPortInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new OutEventPortInstance(factory, is_temp_node);
    });
}

OutEventPortInstance::OutEventPortInstance(EntityFactory& factory, bool is_temp) : EventPortAssembly(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::OUTEVENTPORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    auto data_middleware = factory.AttachData(this, "middleware", QVariant::String, "ZMQ", true);
    connect(data_middleware, &Data::dataChanged, this, &OutEventPortInstance::updateQOSEdge);
    
    updateQOSEdge();
}

void OutEventPortInstance::updateQOSEdge(){
    auto middleware_value = getDataValue("middleware").toString();
    QSet<QString> qos_middlewares = {"RTI", "OSPL"};
    
    //Check for QOSable Middlewares
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE, qos_middlewares.contains(middleware_value));
}