#include "member.h"

Member::Member(): Node(NODE_KIND::MEMBER)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    
    QList<QVariant> types;
    types << "String";
    types << "Boolean";
    types << "Integer";
    types << "Double";
    types << "Float";
    types << "Character";

    addValidValues("type", types);
    updateDefaultData("type", QVariant::String, false, "String");
    updateDefaultData("key", QVariant::Bool, false, false);
}


bool Member::canAdoptChild(Node*)
{
    return false;
}

bool Member::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
