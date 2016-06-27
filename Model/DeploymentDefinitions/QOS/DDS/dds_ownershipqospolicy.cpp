#include "dds_ownershipqospolicy.h"

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy():Node(NT_QOS)
{
}

DDS_OwnershipQosPolicy::~DDS_OwnershipQosPolicy()
{

}

bool DDS_OwnershipQosPolicy::canAdoptChild(Node*)
{
    return false;
}
