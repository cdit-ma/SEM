#include "componentinstance.h"
#include <QDebug>
#include "outeventportinstance.h"
#include "ineventportinstance.h"
#include "attributeinstance.h"
#include "../InterfaceDefinitions/component.h"
#include "../edge.h"
#include "hardwarenode.h"

ComponentInstance::ComponentInstance(QString name):Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed ComponentInstance: "<< this->getName();
}

ComponentInstance::~ComponentInstance()
{
    //THIS IS A TEST
    //Destructor
}

bool ComponentInstance::canAdoptChild(Node *child)
{
    OutEventPortInstance* outEventPortInstance = dynamic_cast<OutEventPortInstance*>(child);
    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(child);
    AttributeInstance* attributeInstance = dynamic_cast<AttributeInstance*>(child);

    if(!(outEventPortInstance || inEventPortInstance || attributeInstance)){
        qCritical() << "Component Instance can only Adopt a OutEventPortInstance, InEventPortInstance or AttributeInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}


bool ComponentInstance::canConnect(Node* attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);
    Component* component = dynamic_cast<Component*> (attachableObject);

    if(hardwareNode == 0 && component == 0){
        qWarning() << "ComponentInstance Node can only be connected to a HardwareNode";
        return false;
    }

    //Check for an edge to a component.
    foreach(Edge* edge, getEdges(0)){
        Component* src = dynamic_cast<Component*>(edge->getSource());
        Component* dst = dynamic_cast<Component*>(edge->getDestination());
        if(src != 0 || dst != 0){
            //Already connection to a Component. Return 0
            qWarning() << "ComponentInstance Node can only be connected to one Component.";
            return false;
        }
    }

    return Node::canConnect(attachableObject);;
}


QString ComponentInstance::toString()
{
    return QString("ComponentInstance[%1]: "+this->getName()).arg(this->getID());
}

