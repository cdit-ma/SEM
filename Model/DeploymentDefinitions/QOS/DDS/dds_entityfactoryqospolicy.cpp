#include "dds_entityfactoryqospolicy.h"

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy():Node(NT_QOS)
{
}

DDS_EntityFactoryQosPolicy::~DDS_EntityFactoryQosPolicy()
{
}

bool DDS_EntityFactoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}
