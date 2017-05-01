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
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);

     setDefinitionKind(NODE_KIND::BLACKBOX);
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

bool BlackBoxInstance::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
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
