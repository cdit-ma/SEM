#include "dds_resourcelimitsqospolicy.h"

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy():Node(NT_QOS)
{
}

DDS_ResourceLimitsQosPolicy::~DDS_ResourceLimitsQosPolicy()
{
}

bool DDS_ResourceLimitsQosPolicy::canAdoptChild(Node*)
{
    return false;
}
