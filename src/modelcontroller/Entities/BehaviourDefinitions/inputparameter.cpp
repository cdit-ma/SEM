#include "inputparameter.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER;
const QString kind_string = "InputParameter";

void InputParameter::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new InputParameter(factory, is_temp_node);
        });
}

InputParameter::InputParameter(EntityFactory& factory, bool is_temp) : Parameter(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(true);
    setDataProducer(false);

    //Setup Data
    factory.AttachData(this, "value", QVariant::String, "", false);
}