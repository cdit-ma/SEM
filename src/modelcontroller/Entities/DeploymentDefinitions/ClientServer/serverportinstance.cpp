#include "serverportinstance.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::SERVER_PORT_INSTANCE;
const QString kind_string = "ServerPortInstance";

void MEDEA::ServerPortInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ServerPortInstance(factory, is_temp_node);
        });
}

MEDEA::ServerPortInstance::ServerPortInstance(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::SERVER_PORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "index", QVariant::Int, -1, false);
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