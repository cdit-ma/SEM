#include "voidtype.h"

const NODE_KIND node_kind = NODE_KIND::VOID_TYPE;
const QString kind_string = "VoidType";

VoidType::VoidType(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new VoidType();});
};

VoidType::VoidType(): Node(node_kind)
{
}


bool VoidType::canAdoptChild(Node*)
{
    return false;
}

bool VoidType::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
