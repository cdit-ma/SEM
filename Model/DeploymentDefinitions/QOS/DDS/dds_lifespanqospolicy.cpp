#include "dds_lifespanqospolicy.h"

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy():Node(NT_QOS)
{
}

DDS_LifespanQosPolicy::~DDS_LifespanQosPolicy()
{

}

bool DDS_LifespanQosPolicy::canAdoptChild(Node*)
{
    return false;
}
