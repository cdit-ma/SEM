#include "memberinstance.h"
#include "../entityfactory.h"

MemberInstance::MemberInstance(EntityFactory* factory) : DataNode(factory, NODE_KIND::MEMBER_INSTANCE, "MemberInstance"){
	auto node_kind = NODE_KIND::MEMBER_INSTANCE;
	QString kind_string = "MemberInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new MemberInstance();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

MemberInstance::MemberInstance():DataNode(NODE_KIND::MEMBER_INSTANCE)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setDataReciever(true);
    setDataProducer(true);

    setDefinitionKind(NODE_KIND::MEMBER);
}


bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}

bool MemberInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(!(dst->getNodeKind() == NODE_KIND::MEMBER || dst->getNodeKind() == NODE_KIND::MEMBER_INSTANCE)){
                return false;
            }
            break;
        }
    case EDGE_KIND::WORKFLOW:{
        return false;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
