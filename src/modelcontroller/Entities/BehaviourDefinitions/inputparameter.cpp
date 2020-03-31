#include "inputparameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER;
const QString kind_string = "Input Parameter";

void InputParameter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new InputParameter(broker, is_temp_node);
        });
}

InputParameter::InputParameter(EntityFactoryBroker& broker, bool is_temp) : Parameter(broker, node_kind, is_temp){
    //Setup State
    setDataReceiver(true);
    setDataProducer(false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Value, QVariant::String, ProtectedState::UNPROTECTED);
}