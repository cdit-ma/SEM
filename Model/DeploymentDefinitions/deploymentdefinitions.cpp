#include "deploymentdefinitions.h"
#include <QDebug>
#include "assemblydefinitions.h"
#include "hardwaredefinitions.h"

DeploymentDefinitions::DeploymentDefinitions():Node()
{
     addAspect("Assembly");
     addAspect("Hardware");
}

DeploymentDefinitions::~DeploymentDefinitions()
{

}

QString DeploymentDefinitions::toString()
{
    return QString("DeploymentDefinitions[%1]: "+this->getName()).arg(this->getID());
}

bool DeploymentDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool DeploymentDefinitions::canAdoptChild(Node *child)
{

    AssemblyDefinitions* assemblyDefinitions = dynamic_cast<AssemblyDefinitions *>(child);
    HardwareDefinitions* hardwareDefinitions = dynamic_cast<HardwareDefinitions *>(child);


    if(!hardwareDefinitions && !assemblyDefinitions){
        qWarning() << "Cannot Adopt anything outside of Assembly Definitions and Hardware Definitions";
        return false;
    }

    return Node::canAdoptChild(child);
}
