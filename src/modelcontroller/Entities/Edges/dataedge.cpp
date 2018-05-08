#include "dataedge.h"
#include "../node.h"
#include "../../edgekinds.h"

auto edge_kind = EDGE_KIND::DATA;
QString kind_string = "Edge_Data";

DataNode::RegisterWithEntityFactory(EntityFactory& factory, Node *src, Node *dst) : Edge(factory, src, dst, edge_kind){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory){return new DataNode(factory, src, dst);});
}

DataNode::DataNode(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, edge_kind){
};