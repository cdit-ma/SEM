#include "replierportinst.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_REPLIER_INST;
const QString kind_string = "Replier Port Instance";

void MEDEA::ReplierPortInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ReplierPortInst(broker, is_temp_node);
        });
}

MEDEA::ReplierPortInst::ReplierPortInst(::EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, false){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PORT_REPLIER);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    ///Setup Data
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 0);
    
    auto data_middleware = broker.AttachData(this, KeyName::Middleware, QVariant::String, ProtectedState::UNPROTECTED);
    data_middleware->addValidValues({"TAO", "ZMQ", "QPID"});

    connect(data_middleware, &Data::dataChanged, this, &MEDEA::ReplierPortInst::MiddlewareUpdated);
    MiddlewareUpdated();
}

