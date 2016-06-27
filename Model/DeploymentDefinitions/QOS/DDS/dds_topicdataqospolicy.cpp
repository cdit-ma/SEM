#include "dds_topicdataqospolicy.h"

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy():Node(NT_QOS)
{
}

DDS_TopicDataQosPolicy::~DDS_TopicDataQosPolicy()
{

}

bool DDS_TopicDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}
