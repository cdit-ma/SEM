#include "deploymentdefinitions.h"
#include <QDebug>
#include "componentassembly.h"

DeploymentDefinitions::DeploymentDefinitions(QString name):Node(name)
{

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
    return false;
}

bool DeploymentDefinitions::canAdoptChild(Node *child)
{

    ComponentAssembly* component = dynamic_cast<ComponentAssembly *>(child);


    if(component == 0){
        //qWarning() << "Deployment Definitions can only adopt a ComponentAssembly Node";
        //return true;
    }

    return true;
}
