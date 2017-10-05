#include "enummember.h"
#include <QDebug>

EnumMember::EnumMember(EntityFactory* factory) : Node(factory, NODE_KIND::ENUM_MEMBER, "EnumMember"){
	auto node_kind = NODE_KIND::ENUM_MEMBER;
    QString kind_string = "EnumMember";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new EnumMember();});
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
};

EnumMember::EnumMember():Node(NODE_KIND::ENUM_MEMBER)
{
    //setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DATA);
    //setDataProducer(true);
}

bool EnumMember::canAdoptChild(Node*)
{
    return false;
}

bool EnumMember::canAcceptEdge(EDGE_KIND kind, Node *dst)
{
    qCritical() << "ENUM MEMBER CAN ACCEPT EDGE";
    switch(kind){
    case EDGE_KIND::DATA:{
        bool allow_edge = false;
        if(dst->getNodeKind() == NODE_KIND::ENUM_INSTANCE){
            auto enum_def = dst->getDefinition(true);
            if(enum_def && enum_def->isAncestorOf(this) && getViewAspect() != dst->getViewAspect()){
                qCritical() << "GOT ENUM!";
                allow_edge = true;
                return true;
            }
        }
        if(!allow_edge){
            return false;
        }else{
            return Node::canAcceptEdge(kind, dst);
        }
    }
    default:
        break;
    }
    return false;
}