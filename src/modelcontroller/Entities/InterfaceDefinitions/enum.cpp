#include "enum.h"


Enum::Enum(EntityFactory* factory) : Node(factory, NODE_KIND::ENUM, "Enum"){
	auto node_kind = NODE_KIND::ENUM;
	QString kind_string = "Enum";
    RegisterNodeKind(factory, node_kind, kind_string, [](){return new Enum();});
};

Enum::Enum():Node(NODE_KIND::ENUM)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setInstanceKind(NODE_KIND::ENUM_INSTANCE);
    
}

bool Enum::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ENUM_MEMBER:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Enum::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}