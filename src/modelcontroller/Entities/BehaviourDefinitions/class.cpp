#include "class.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::CLASS;
const QString kind_string = "Class";

void MEDEA::Class::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::Class(factory, is_temp_node);
        });
}

MEDEA::Class::Class(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER);
    addInstanceKind(NODE_KIND::CLASS_INSTANCE);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);

    //Setup Data
    factory.AttachData(this, "icon_prefix", QVariant::String, "", false);
    factory.AttachData(this, "icon", QVariant::String, "", false);
};