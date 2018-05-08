#include "variableparameter.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::VARIABLE_PARAMETER;
const QString kind_string = "VariableParameter";

void VariableParameter::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new VariableParameter(factory, is_temp_node);
        });
}

VariableParameter::VariableParameter(EntityFactory& factory, bool is_temp) : Parameter(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(true);
    setDataProducer(true);

    //Setup Data
    factory.AttachData(this, "value", QVariant::String, "", false);
}