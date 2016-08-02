#include "dds_latencybudgetqospolicy.h"

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy():Node(NT_QOS)
{
}

DDS_LatencyBudgetQosPolicy::~DDS_LatencyBudgetQosPolicy()
{
}

bool DDS_LatencyBudgetQosPolicy::canAdoptChild(Node*)
{
    return false;
}
