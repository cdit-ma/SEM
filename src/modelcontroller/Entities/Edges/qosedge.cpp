#include "qosedge.h"
#include "../../entityfactory.h"

const auto edge_kind = EDGE_KIND::QOS;
const QString kind_string = "Edge_Qos";

void QosEdge::RegisterWithEntityFactory(EntityFactory& factory){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory, Node* src, Node* dst){
        return new QosEdge(factory, src, dst);
        });
}

QosEdge::QosEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, src, dst, edge_kind){
}