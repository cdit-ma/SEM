#include "code.h"

const NODE_KIND node_kind = NODE_KIND::CODE;
const QString kind_string = "Code";


Code::Code(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Code();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "code", QVariant::String);
};

Code::Code() : Node(node_kind){
}

bool Code::canAdoptChild(Node*)
{
    return false;
}

bool Code::canAcceptEdge(EDGE_KIND, Node*)
{
    return false;
}
