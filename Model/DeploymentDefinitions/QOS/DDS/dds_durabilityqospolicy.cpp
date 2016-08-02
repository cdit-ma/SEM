#include "dds_durabilityqospolicy.h"

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy():Node(NT_QOS)
{
}

DDS_DurabilityQosPolicy::~DDS_DurabilityQosPolicy()
{

}

bool DDS_DurabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}
