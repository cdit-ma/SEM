#include "managementcomponent.h"
#include "hardwarenode.h"
#include <QDebug>

ManagementComponent::ManagementComponent()
{
}


ManagementComponent::~ManagementComponent()
{

}

bool ManagementComponent::canConnect(Node* attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode *>(attachableObject);

    if(!hardwareNode){
#ifdef DEBUG_MODE
        qWarning() << "Cannot only connect ManagementComponent to 1 HardwareNode.";
#endif
        return false;
    }

    if(edgeCount() > 0){
#ifdef DEBUG_MODE
        qWarning() << "Cannot only to 1 HardwareNode.";
#endif
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool ManagementComponent::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
