#include "dds_reliabilityqospolicy.h"

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy():Node(NT_QOS)
{
}

DDS_ReliabilityQosPolicy::~DDS_ReliabilityQosPolicy()
{
}

bool DDS_ReliabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}
