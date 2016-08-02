#include "dds_livelinessqospolicy.h"

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy():Node(NT_QOS)
{
}

DDS_LivelinessQosPolicy::~DDS_LivelinessQosPolicy()
{

}

bool DDS_LivelinessQosPolicy::canAdoptChild(Node*)
{
    return false;
}
