#include "managementcomponent.h"

ManagementComponent::ManagementComponent():Node(NK_MANAGEMENT_COMPONENT)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

bool ManagementComponent::canAdoptChild(Node*)
{
    return false;
}

bool ManagementComponent::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
