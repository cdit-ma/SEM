#include "clientportinstance.h"

const NODE_KIND node_kind = NODE_KIND::CLIENT_PORT_INSTANCE;
const QString kind_string = "ClientPortInstance";

MEDEA::ClientPortInstance::ClientPortInstance(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ClientPortInstance();});
};

MEDEA::ClientPortInstance::ClientPortInstance(): Node(node_kind)
{
    addInstancesDefinitionKind(NODE_KIND::CLIENT_PORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);
}


bool MEDEA::ClientPortInstance::canAdoptChild(Node* child)
{
    return false;
}

bool MEDEA::ClientPortInstance::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::ASSEMBLY:{
        if(dst->getNodeKind() != NODE_KIND::SERVER_PORT_INSTANCE){
            return false;
        }

        if(!getDefinition()){
            return false;
        }
        
        //Can't connect different aggregates
        if(getDefinition(true) != dst->getDefinition(true)){
            return false;
        }
        
        if(!getEdges(0, edge_kind).isEmpty()){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}

QList<Node*> MEDEA::ClientPortInstance::getAdoptableNodes(Node* definition){
    return QList<Node*>();
}
