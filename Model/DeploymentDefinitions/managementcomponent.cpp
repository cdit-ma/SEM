#include "managementcomponent.h"

ManagementComponent::ManagementComponent()
{
    setAcceptEdgeClass(Edge::EC_DEPLOYMENT);
}

ManagementComponent::~ManagementComponent()
{
}

bool ManagementComponent::canAdoptChild(Node*)
{
    return false;
}
