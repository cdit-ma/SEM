#include "dds_deadlineqospolicy.h"

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NT_QOS)
{
}

DDS_DeadlineQosPolicy::~DDS_DeadlineQosPolicy()
{
}

bool DDS_DeadlineQosPolicy::canAdoptChild(Node*)
{
    return false;
}
