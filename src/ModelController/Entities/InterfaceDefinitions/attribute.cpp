#include "attribute.h"


Attribute::Attribute(EntityFactory* factory) : Node(factory, NODE_KIND::ATTRIBUTE, "Attribute"){
	auto node_kind = NODE_KIND::ATTRIBUTE;
	QString kind_string = "Attribute";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Attribute();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
};

Attribute::Attribute():Node(NODE_KIND::ATTRIBUTE)
{
    setNodeType(NODE_TYPE::DEFINITION);

    setInstanceKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    setImplKind(NODE_KIND::ATTRIBUTE_IMPL);
}

bool Attribute::canAdoptChild(Node*)
{
    return false;
}

bool Attribute::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}