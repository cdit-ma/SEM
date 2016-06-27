#include "dds_userdataqospolicy.h"

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy():Node(NT_QOS)
{
}

DDS_UserDataQosPolicy::~DDS_UserDataQosPolicy()
{

}

bool DDS_UserDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}
