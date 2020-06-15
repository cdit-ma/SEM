#include "qosedge.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const auto edge_kind = EDGE_KIND::QOS;
const QString kind_string = "Edge_Qos";

void QosEdge::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(edge_kind, kind_string, [](EntityFactoryBroker& broker, Node* src, Node* dst){
        return new QosEdge(broker, src, dst);
        });
}

QosEdge::QosEdge(EntityFactoryBroker& broker, Node* src, Node* dst) : Edge(broker, src, dst, edge_kind){
}