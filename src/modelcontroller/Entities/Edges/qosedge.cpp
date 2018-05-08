#include "qosedge.h"
#include "../../edgekinds.h"

const auto edge_kind = EDGE_KIND::QOS;
const QString kind_string = "Edge_Qos";

QosEdge::RegisterWithEntityFactory(EntityFactory& factory, Node *src, Node *dst) : Edge(factory, src, dst, edge_kind){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory){return new QosEdge(factory, src, dst);});
}

QosEdge::QosEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, edge_kind){
}