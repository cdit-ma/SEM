#include "behaviourdefinitions.h"

BehaviourDefinitions::BehaviourDefinitions():Node(Node::NK_BEHAVIOUR_DEFINITIONS)
{
    setNodeType(NT_ASPECT);
}

VIEW_ASPECT BehaviourDefinitions::getViewAspect() const
{
    return VA_BEHAVIOUR;
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case Node::NK_COMPONENT_IMPL:{
        break;
    }
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool BehaviourDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
