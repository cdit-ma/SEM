#include "dds_writerdatalifecycleqospolicy.h"

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy():Node(NT_QOS)
{
}

DDS_WriterDataLifecycleQosPolicy::~DDS_WriterDataLifecycleQosPolicy()
{

}

bool DDS_WriterDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}
