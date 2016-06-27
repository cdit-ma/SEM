#include "dds_readerdatalifecycleqospolicy.h"

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy():Node(NT_QOS)
{
}

DDS_ReaderDataLifecycleQosPolicy::~DDS_ReaderDataLifecycleQosPolicy()
{

}

bool DDS_ReaderDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}
