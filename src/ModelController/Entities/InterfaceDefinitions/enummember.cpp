#include "enummember.h"


EnumMember::EnumMember(EntityFactory* factory) : Node(factory, NODE_KIND::ENUM_MEMBER, "EnumMember"){
	auto node_kind = NODE_KIND::ENUM_MEMBER;
    QString kind_string = "EnumMember";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new EnumMember();});
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
};

EnumMember::EnumMember():Node(NODE_KIND::ENUM_MEMBER)
{
    //setNodeType(NODE_TYPE::DEFINITION);
}

bool EnumMember::canAdoptChild(Node*)
{
    return false;
}

bool EnumMember::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}