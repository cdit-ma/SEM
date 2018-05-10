#include "variadicparameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::VARIADIC_PARAMETER;
const QString kind_string = "VariadicParameter";

void VariadicParameter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new VariadicParameter(broker, is_temp_node);
        });
}

VariadicParameter::VariadicParameter(EntityFactoryBroker& broker, bool is_temp) : Parameter(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(true);
    setDataProducer(false);



    
    //Setup Data
    broker.AttachData(this, "value", QVariant::String, "", false);
}