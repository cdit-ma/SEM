#include "enummember.h"
#include "enuminstance.h"
#include <QDebug>

EnumMember::EnumMember(EntityFactory* factory) : DataNode(factory, NODE_KIND::ENUM_MEMBER, "EnumMember"){
	auto node_kind = NODE_KIND::ENUM_MEMBER;
    QString kind_string = "EnumMember";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new EnumMember();});
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
};

EnumMember::EnumMember():DataNode(NODE_KIND::ENUM_MEMBER)
{
    setDataProducer(true);
}

bool EnumMember::canAdoptChild(Node*)
{
    return false;
}

bool EnumMember::canAcceptEdge(EDGE_KIND kind, Node *dst)
{
    switch(kind){
    case EDGE_KIND::DATA:{
        bool allow_edge = false;
        if(dst->getNodeKind() == NODE_KIND::ENUM_INSTANCE){
            auto enum_instance = (EnumInstance*)dst;
            auto enum_def = dst->getDefinition(true);
            if(enum_def && enum_def->isAncestorOf(this) && getViewAspect() != dst->getViewAspect() && !enum_instance->hasInputData()){
                allow_edge = true;
            }
        }
        if(!allow_edge){
            return false;
        }else{
            return true;
        }
    }
    default:
        break;
    }
    return Node::canAcceptEdge(kind, dst);
}