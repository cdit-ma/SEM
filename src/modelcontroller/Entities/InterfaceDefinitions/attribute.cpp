#include "attribute.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE;
const QString kind_string = "Attribute";

void Attribute::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Attribute(factory, is_temp_node);
    });
}

Attribute::Attribute(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstanceKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    addImplKind(NODE_KIND::ATTRIBUTE_IMPL);

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "String", true);
}