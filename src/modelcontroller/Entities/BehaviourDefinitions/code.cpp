#include "code.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::CODE;
const QString kind_string = "Code";

void Code::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Code(factory, is_temp_node);
        });
}

Code::Code(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);

    //Setup Data
    factory.AttachData(this, "code", QVariant::String);
}
