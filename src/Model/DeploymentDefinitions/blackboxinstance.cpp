#include "blackboxinstance.h"
#include "../entityfactory.h"

BlackBoxInstance::BlackBoxInstance(EntityFactory* factory) : Node(factory, NODE_KIND::BLACKBOX_INSTANCE, "BlackBoxInstance"){
	auto node_kind = NODE_KIND::BLACKBOX_INSTANCE;
	QString kind_string = "BlackBoxInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BlackBoxInstance();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

BlackBoxInstance::BlackBoxInstance():Node(NODE_KIND::BLACKBOX_INSTANCE)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

bool BlackBoxInstance::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Instances
    switch(child->getNodeKind()){
    case NODE_KIND::INEVENTPORT_INSTANCE:
    case NODE_KIND::OUTEVENTPORT_INSTANCE:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool BlackBoxInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::BLACKBOX){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
