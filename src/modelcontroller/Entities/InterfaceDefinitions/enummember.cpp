#include "enummember.h"
#include "enuminstance.h"
#include <QDebug>

EnumMember::EnumMember(EntityFactory* factory) : DataNode(factory, NODE_KIND::ENUM_MEMBER, "EnumMember"){
	auto node_kind = NODE_KIND::ENUM_MEMBER;
    QString kind_string = "EnumMember";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new EnumMember();});
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

EnumMember::EnumMember():DataNode(NODE_KIND::ENUM_MEMBER)
{
    setPromiscuousDataLinker(true);
    setDataProducer(true);
}

bool EnumMember::canAdoptChild(Node*)
{
    return false;
}

bool EnumMember::canAcceptEdge(EDGE_KIND kind, Node *dst)
{
    return DataNode::canAcceptEdge(kind, dst);
}