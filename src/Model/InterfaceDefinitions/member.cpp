#include "member.h"
#include "../entityfactory.h"

Member::Member(EntityFactory* factory) : Node(factory, NODE_KIND::MEMBER, "Member"){
	auto node_kind = NODE_KIND::MEMBER;
	QString kind_string = "Member";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Member();});

    QList<QVariant> types;
    types << "String";
    types << "Boolean";
    types << "Integer";
    types << "Double";
    types << "Float";
    types << "Character";

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
    RegisterDefaultData(factory, node_kind, "key", QVariant::String, false, false);
};

Member::Member(): Node(NODE_KIND::MEMBER)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
}


bool Member::canAdoptChild(Node*)
{
    return false;
}

bool Member::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
