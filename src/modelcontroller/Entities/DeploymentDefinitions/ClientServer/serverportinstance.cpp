#include "serverportinstance.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_PORT_INSTANCE;
const QString kind_string = "ServerPortInstance";

MEDEA::ServerPortInstance::ServerPortInstance(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ServerPortInstance();});
};

MEDEA::ServerPortInstance::ServerPortInstance(): Node(node_kind)
{
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setDefinitionKind(NODE_KIND::SERVER_PORT);

    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY);
}


bool MEDEA::ServerPortInstance::canAdoptChild(Node* child)
{
    return false;
}

bool MEDEA::ServerPortInstance::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(!acceptsEdgeKind(edge_kind)){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::ASSEMBLY:{
        //Can't connect different aggregates
        if(getDefinition() != dst->getDefinition()){
            return false;
        }
        if(!getDefinition()){
            return false;
        }
        break;
    }
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::SERVER_PORT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}


QList<Node*> MEDEA::ServerPortInstance::getAdoptableNodes(Node* definition){
    return QList<Node*>();
}