#include "variadicparameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::VARIADIC_PARAMETER;
const QString kind_string = "Variadic Parameter";

void VariadicParameter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new VariadicParameter(broker, is_temp_node);
        });
}

VariadicParameter::VariadicParameter(EntityFactoryBroker& broker, bool is_temp) : Parameter(broker, node_kind, is_temp){
    //Setup State
    setDataReceiver(true);
    setDataProducer(false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.RemoveData(this, "label");
    broker.RemoveData(this, "icon");
    broker.RemoveData(this, "icon_prefix");
    broker.AttachData(this, "value", QVariant::String, ProtectedState::UNPROTECTED);
}