#include "hardwarecluster.h"
#include "hardwarenode.h"

HardwareCluster::HardwareCluster():Hardware(false)
{
}

HardwareCluster::~HardwareCluster()
{
}

bool HardwareCluster::canAdoptChild(Node *child)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (child);

    if(!hardwareNode){
        return false;
    }

    return Hardware::canAdoptChild(child);
}
