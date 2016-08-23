#include "dds_userdataqospolicy.h"

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy():Node(NK_QOS_DDS_POLICY_USERDATA)
{
    setNodeType(NT_QOS_DDS_POLICY);
}


bool DDS_UserDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_UserDataQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
