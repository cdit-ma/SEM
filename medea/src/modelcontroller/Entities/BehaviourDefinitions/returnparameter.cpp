#include "returnparameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER;
const QString kind_string = "Return Parameter";

void ReturnParameter::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ReturnParameter(broker, is_temp_node);
        });
}

ReturnParameter::ReturnParameter(EntityFactoryBroker& broker, bool is_temp) : Parameter(broker, node_kind, is_temp){
    //Setup State
    setDataReceiver(false);
    setDataProducer(true);
}