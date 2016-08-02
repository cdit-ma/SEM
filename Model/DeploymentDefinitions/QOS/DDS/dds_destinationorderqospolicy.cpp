#include "dds_destinationorderqospolicy.h"

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy():Node(NT_QOS)
{
}

DDS_DestinationOrderQosPolicy::~DDS_DestinationOrderQosPolicy()
{
}

bool DDS_DestinationOrderQosPolicy::canAdoptChild(Node*)
{
    return false;
}
