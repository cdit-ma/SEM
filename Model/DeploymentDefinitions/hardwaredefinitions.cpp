#include "hardwaredefinitions.h"
#include "hardwarecluster.h"
#include <QDebug>

HardwareDefinitions::HardwareDefinitions():Node()
{
}

HardwareDefinitions::~HardwareDefinitions()
{

}

QString HardwareDefinitions::toString()
{
    return QString("HardwareDefinitions[%1]: "+this->getName()).arg(this->getID());
}

bool HardwareDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool HardwareDefinitions::canAdoptChild(Node *child)
{

    HardwareCluster* hardwareCluster = dynamic_cast<HardwareCluster *>(child);


    if(!hardwareCluster){
        qWarning() << "Deployment Definitions can only adopt a ComponentAssembly Node";
        return false;
    }

    return Node::canAdoptChild(child);
}
