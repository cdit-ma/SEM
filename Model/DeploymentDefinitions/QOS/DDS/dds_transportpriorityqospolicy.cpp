#include "dds_transportpriorityqospolicy.h"

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy():Node(NT_QOS)
{
}

DDS_TransportPriorityQosPolicy::~DDS_TransportPriorityQosPolicy()
{

}

bool DDS_TransportPriorityQosPolicy::canAdoptChild(Node*)
{
    return false;
}
