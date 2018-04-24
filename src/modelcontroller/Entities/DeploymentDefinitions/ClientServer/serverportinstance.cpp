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
    addInstancesDefinitionKind(NODE_KIND::SERVER_PORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);
}


bool MEDEA::ServerPortInstance::canAdoptChild(Node* child)
{
    return false;
}

bool MEDEA::ServerPortInstance::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
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
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}


QList<Node*> MEDEA::ServerPortInstance::getAdoptableNodes(Node* definition){
    return QList<Node*>();
}