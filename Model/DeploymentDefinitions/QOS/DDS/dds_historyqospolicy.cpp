#include "dds_historyqospolicy.h"

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy():Node(NT_QOS)
{
}

DDS_HistoryQosPolicy::~DDS_HistoryQosPolicy()
{

}

bool DDS_HistoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}
