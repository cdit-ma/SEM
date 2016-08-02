#include "dds_durabilityserviceqospolicy.h"

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy():Node(NT_QOS)
{
}

DDS_DurabilityServiceQosPolicy::~DDS_DurabilityServiceQosPolicy()
{
}

bool DDS_DurabilityServiceQosPolicy::canAdoptChild(Node*)
{
    return false;
}
