#include "hardwaredefinitions.h"
#include "hardwarecluster.h"
#include "hardwarenode.h"
#include <QDebug>

HardwareDefinitions::HardwareDefinitions():Node()
{
}

HardwareDefinitions::~HardwareDefinitions()
{

}

bool HardwareDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool HardwareDefinitions::canAdoptChild(Node *child)
{

    HardwareCluster* hardwareCluster = dynamic_cast<HardwareCluster *>(child);
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode *>(child);


    if(!hardwareCluster && !hardwareNode){
#ifdef DEBUG_MODE
        qWarning() << "HardwareDefinitions can only adopt a HardwareCluster Node/ HardwareNode";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}
