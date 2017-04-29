#include "interfacedefinitions.h"

InterfaceDefinitions::InterfaceDefinitions(): Node(NK_INTERFACE_DEFINITIONS)
{
    setNodeType(NT_ASPECT);

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
    case NK_IDL:
        break;
    case NK_AGGREGATE:
        break;
    case NK_COMPONENT:
    case NK_COMPONENT_IMPL:
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
