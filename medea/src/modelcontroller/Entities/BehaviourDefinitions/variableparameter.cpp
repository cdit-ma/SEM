#include "variableparameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::VARIABLE_PARAMETER;
const QString kind_string = "Variable Parameter";

void VariableParameter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new VariableParameter(broker, is_temp_node);
        });
}

VariableParameter::VariableParameter(EntityFactoryBroker& broker, bool is_temp) : Parameter(broker, node_kind, is_temp){
    //Setup State
    setDataReceiver(true);
    setDataProducer(true);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED);
    broker.RemoveData(this, KeyName::InnerType);
    broker.RemoveData(this, KeyName::OuterType);
}