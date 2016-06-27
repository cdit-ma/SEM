#include "dds_timebasedfilterqospolicy.h"

DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy():Node(NT_QOS)
{
}

DDS_TimeBasedFilterQosPolicy::~DDS_TimeBasedFilterQosPolicy()
{
}

bool DDS_TimeBasedFilterQosPolicy::canAdoptChild(Node*)
{
    return false;
}
