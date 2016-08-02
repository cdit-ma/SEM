#include "dds_partitionqospolicy.h"

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy():Node(NT_QOS)
{
}

DDS_PartitionQosPolicy::~DDS_PartitionQosPolicy()
{
}

bool DDS_PartitionQosPolicy::canAdoptChild(Node*)
{
    return false;
}
