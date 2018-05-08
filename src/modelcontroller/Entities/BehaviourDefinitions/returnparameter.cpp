#include "returnparameter.h"
#include "../../entityfactory.h"
#include "../../edgekinds.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER;
const QString kind_string = "ReturnParameter";

void ReturnParameter::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new ReturnParameter(factory, is_temp_node);
        });
}

ReturnParameter::ReturnParameter(EntityFactory& factory, bool is_temp) : Parameter(node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(false);
    setDataProducer(true);
}