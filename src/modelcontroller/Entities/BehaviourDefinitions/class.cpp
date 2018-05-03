#include "class.h"

const NODE_KIND node_kind = NODE_KIND::CLASS;
const QString kind_string = "Class";

MEDEA::Class::Class(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Class();});
    
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, false);
};

MEDEA::Class::Class(): Node(node_kind)
{
    setNodeType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER);
    addInstanceKind(NODE_KIND::CLASS_INSTANCE);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);

}