#include "hardwaredefinitions.h"
#include "hardware.h"

HardwareDefinitions::HardwareDefinitions():Node(Node::NT_ASPECT)
{
}

HardwareDefinitions::~HardwareDefinitions()
{
}

VIEW_ASPECT HardwareDefinitions::getViewAspect()
{
    return VA_HARDWARE;
}

bool HardwareDefinitions::canAdoptChild(Node *child)
{
    Hardware* hardware = dynamic_cast<Hardware*>(child);

    if(!hardware){
        return false;
    }

    return Node::canAdoptChild(child);
}
