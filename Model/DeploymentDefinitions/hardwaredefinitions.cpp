#include "hardwaredefinitions.h"
#include "hardware.h"

HardwareDefinitions::HardwareDefinitions():Node(Node::NT_ASPECT)
{
}

HardwareDefinitions::~HardwareDefinitions()
{
}

bool HardwareDefinitions::canAdoptChild(Node *child)
{
    Hardware* hardware = dynamic_cast<Hardware*>(child);

    if(!hardware){
        return false;
    }

    return Node::canAdoptChild(child);
}
