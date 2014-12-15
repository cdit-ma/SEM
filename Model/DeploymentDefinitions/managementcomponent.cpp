#include "managementcomponent.h"
#include "hardwarenode.h"
#include <QDebug>

ManagementComponent::ManagementComponent()
{
    addAspect("Assembly");
}


ManagementComponent::~ManagementComponent()
{

}

QString ManagementComponent::toString()
{
    return QString("ManagementComponent[%1]: "+this->getName()).arg(this->getID());
}

bool ManagementComponent::canConnect(Node* attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode *>(attachableObject);

    if(!hardwareNode){
        qWarning() << "Cannot only connect ManagementComponent to 1 HardwareNode.";
        return false;
    }

    if(edgeCount() > 0){
        qWarning() << "Cannot only to 1 HardwareNode.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool ManagementComponent::canAdoptChild(Node *child)
{

    return false;
}
