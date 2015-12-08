#include "hardwarenode.h"

HardwareNode::HardwareNode():Hardware(true)
{
}

HardwareNode::~HardwareNode()
{
}

bool HardwareNode::canAdoptChild(Node*)
{
    return false;
}
