#include "attribute.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE;
const QString kind_string = "Attribute";


Attribute::Attribute(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Attribute();});
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
};

Attribute::Attribute() : Node(node_kind)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setInstanceKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    setImplKind(NODE_KIND::ATTRIBUTE_IMPL);
    
}

bool Attribute::canAdoptChild(Node*)
{
    return false;
}

bool Attribute::canAcceptEdge(EDGE_KIND, Node *dst)
{
    return false;
}