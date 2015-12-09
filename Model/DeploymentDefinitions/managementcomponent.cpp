#include "managementcomponent.h"

ManagementComponent::ManagementComponent()
{
    addValidEdgeType(Edge::EC_DEPLOYMENT);
}

ManagementComponent::~ManagementComponent()
{
}

bool ManagementComponent::canAdoptChild(Node*)
{
    return false;
}
