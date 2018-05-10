#include "variableparameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::VARIABLE_PARAMETER;
const QString kind_string = "VariableParameter";

void VariableParameter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new VariableParameter(broker, is_temp_node);
        });
}

VariableParameter::VariableParameter(EntityFactoryBroker& broker, bool is_temp) : Parameter(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(true);
    setDataProducer(true);


    
    
    //Setup Data
    broker.AttachData(this, "value", QVariant::String, "", false);
}