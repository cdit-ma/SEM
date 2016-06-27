#include "dds_presentationqospolicy.h"

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy():Node(NT_QOS)
{
}

DDS_PresentationQosPolicy::~DDS_PresentationQosPolicy()
{

}

bool DDS_PresentationQosPolicy::canAdoptChild(Node*)
{
    return false;
}
