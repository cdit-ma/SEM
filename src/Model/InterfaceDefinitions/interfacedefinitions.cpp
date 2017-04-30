#include "interfacedefinitions.h"

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
