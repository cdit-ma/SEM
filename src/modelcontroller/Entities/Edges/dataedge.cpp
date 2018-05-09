#include "dataedge.h"
#include "../node.h"
#include "../../entityfactory.h"

const static EDGE_KIND edge_kind = EDGE_KIND::DATA;
const static QString kind_string = "Edge_Data";

void DataEdge::RegisterWithEntityFactory(EntityFactory& factory){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory, Node* src, Node* dst){
        return new DataEdge(factory, src, dst);
        });
}

DataEdge::DataEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, src, dst, edge_kind){
};