#include "variadicparameter.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::VARIADIC_PARAMETER;
const QString kind_string = "VariadicParameter";

void VariadicParameter::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new VariadicParameter(factory, is_temp_node);
        });
}

VariadicParameter::VariadicParameter(EntityFactory& factory, bool is_temp) : Parameter(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(true);
    setDataProducer(false);

    //Setup Data
    factory.AttachData(this, "value", QVariant::String, "", false);
}