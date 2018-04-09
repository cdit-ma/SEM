#include "header.h"

const NODE_KIND node_kind = NODE_KIND::HEADER;
const QString kind_string = "Header";

Header::Header(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Header();});

    //Register Default Data
    RegisterDefaultData(factory, node_kind, "code", QVariant::String);
    RegisterDefaultData(factory, node_kind, "row", QVariant::Int, true, 1);
    RegisterDefaultData(factory, node_kind, "row_subgroup", QVariant::Int, true, -1);

    RegisterDefaultData(factory, node_kind, "header_location", QVariant::String, false, "CPP");
    QList<QVariant> header_locations = {"Class Declaration", "Header", "CPP"};

    RegisterValidDataValues(factory, node_kind, "header_location", QVariant::String, header_locations);

};

Header::Header():Node(node_kind){

}

bool Header::canAdoptChild(Node*)
{
    return false;
}

bool Header::canAcceptEdge(EDGE_KIND, Node*)
{
    return false;
}
