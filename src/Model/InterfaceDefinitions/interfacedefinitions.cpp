#include "interfacedefinitions.h"

#include "../entityfactory.h"
InterfaceDefinitions::InterfaceDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::INTERFACE_DEFINITIONS, "InterfaceDefinitions"){
	auto node_kind = NODE_KIND::INTERFACE_DEFINITIONS;
	QString kind_string = "InterfaceDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InterfaceDefinitions();});
};

InterfaceDefinitions::InterfaceDefinitions(): Node(NODE_KIND::INTERFACE_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "DATATYPES");
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect() const
{
    return VA_INTERFACES;
}

bool InterfaceDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::IDL:
        break;
    case NODE_KIND::AGGREGATE:
        break;
    case NODE_KIND::COMPONENT:
    case NODE_KIND::COMPONENT_IMPL:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool InterfaceDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
