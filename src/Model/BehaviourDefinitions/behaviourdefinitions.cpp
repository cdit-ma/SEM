#include "behaviourdefinitions.h"
#include "../nodekinds.h"

BehaviourDefinitions::BehaviourDefinitions():Node(NODE_KIND::BEHAVIOUR_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "COMPONENTS");
}

VIEW_ASPECT BehaviourDefinitions::getViewAspect() const
{
    return VA_BEHAVIOUR;
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::COMPONENT_IMPL:{
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
