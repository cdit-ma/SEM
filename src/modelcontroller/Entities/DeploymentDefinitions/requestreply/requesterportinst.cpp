#include "requesterportinst.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_REQUESTER_INST;
const QString kind_string = "Requester Port Instance";

void MEDEA::RequesterPortInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::RequesterPortInst(broker, is_temp_node);
        });
}

MEDEA::RequesterPortInst::RequesterPortInst(::EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, false){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PORT_REQUESTER);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);
    SetEdgeRuleActive(EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);

    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 2);
    
    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"TAO", "ZMQ", "QPID"});
}


bool MEDEA::RequesterPortInst::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::ASSEMBLY:{
        if(getEdgeOfKindCount(edge_kind)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return EventPortAssembly::canAcceptEdge(edge_kind, dst);
}