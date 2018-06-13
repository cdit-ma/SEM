#include "replierportinst.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_REPLIER_INST;
const QString kind_string = "Replier Port Instance";

void MEDEA::ReplierPortInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ReplierPortInst(broker, is_temp_node);
        });
}

MEDEA::ReplierPortInst::ReplierPortInst(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PORT_REPLIER);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    ///Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
    
    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"TAO", "ZMQ", "AMQP"});

    //connect(data_middleware, &Data::dataChanged, this, &SubscriberPortInst::MiddlewareUpdated);
    //MiddlewareUpdated();
}


bool MEDEA::ReplierPortInst::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
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