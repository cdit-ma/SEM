#include "attribute.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE;
const QString kind_string = "Attribute";


Attribute::Attribute(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Attribute();});
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
};

Attribute::Attribute() : Node(node_kind)
{
    addInstanceKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    addImplKind(NODE_KIND::ATTRIBUTE_IMPL);
}