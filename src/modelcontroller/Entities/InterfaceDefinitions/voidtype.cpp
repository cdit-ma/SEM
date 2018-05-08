#include "voidtype.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::VOID_TYPE;
const QString kind_string = "VoidType";

void VoidType::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new VoidType(factory, is_temp_node);
    });
}

VoidType::VoidType(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setChainableDefinition();

    //Setup Data
    setLabelFunctional(false);
    factory.AttachData(this, "label", QVariant::String, "VOID Type", true);
}
