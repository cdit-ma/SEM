#include "dds_groupdataqospolicy.h"

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NT_QOS)
{
}

DDS_GroupDataQosPolicy::~DDS_GroupDataQosPolicy()
{

}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}
