#include "clientportinstance.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::CLIENT_PORT_INSTANCE;
const QString kind_string = "Client Port Instance";

void MEDEA::ClientPortInstance::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ClientPortInstance(broker, is_temp_node);
        });
}

MEDEA::ClientPortInstance::ClientPortInstance(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::CLIENT_PORT);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);
    SetEdgeRuleActive(EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.ProtectData(this, "index", false);
    broker.AttachData(this, "row", QVariant::Int, 2, true);
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
        
        if(getEdgeOfKindCount(edge_kind)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}