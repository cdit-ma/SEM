#include "member.h"


Member::Member(EntityFactory* factory) : Node(factory, NODE_KIND::MEMBER, "Member"){
	auto node_kind = NODE_KIND::MEMBER;
	QString kind_string = "Member";
    

    //Allow reordering
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Member();});

    QList<QVariant> types = {"String",  "Boolean", "Integer", "Double", "Float", "Character"};


    //Register values
    RegisterValidDataValues(factory, node_kind, "type", QVariant::String, types);
    RegisterValidDataValues(factory, NODE_KIND::ATTRIBUTE, "type", QVariant::String, types);
    RegisterValidDataValues(factory, NODE_KIND::DEPLOYMENT_ATTRIBUTE, "type", QVariant::String, types);

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
    RegisterDefaultData(factory, node_kind, "key", QVariant::Bool, false, false);
};

Member::Member(): Node(NODE_KIND::MEMBER)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setInstanceKind(NODE_KIND::MEMBER_INSTANCE);
}


bool Member::canAdoptChild(Node*)
{
    return false;
}

bool Member::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
