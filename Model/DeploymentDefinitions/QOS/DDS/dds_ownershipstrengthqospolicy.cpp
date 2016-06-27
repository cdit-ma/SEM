#include "dds_ownershipstrengthqospolicy.h"

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy():Node(NT_QOS)
{
}

DDS_OwnershipStrengthQosPolicy::~DDS_OwnershipStrengthQosPolicy()
{

}

bool DDS_OwnershipStrengthQosPolicy::canAdoptChild(Node*)
{
    return false;
}
