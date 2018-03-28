#include "interfacedefinitions.h"


InterfaceDefinitions::InterfaceDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::INTERFACE_DEFINITIONS, "InterfaceDefinitions"){
	auto node_kind = NODE_KIND::INTERFACE_DEFINITIONS;
	QString kind_string = "InterfaceDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InterfaceDefinitions();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "INTERFACES");
};

InterfaceDefinitions::InterfaceDefinitions(): Node(NODE_KIND::INTERFACE_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::INTERFACES;
}

bool InterfaceDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::IDL:
    case NODE_KIND::ENUM:
    case NODE_KIND::AGGREGATE:
    case NODE_KIND::COMPONENT:
    case NODE_KIND::SHARED_DATATYPES:
    case NODE_KIND::NAMESPACE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool InterfaceDefinitions::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
