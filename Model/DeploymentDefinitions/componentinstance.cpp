#include "componentinstance.h"
#include <QDebug>
#include "outeventportinstance.h"
#include "ineventportinstance.h"
#include "attributeinstance.h"
#include "hardwarecluster.h"
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


//A ComponentInstance can be connected to:
//Deployed to:
// + HardwareCluster
// + HardwareNode
//Connected to a Definition:
// + Component (If it has no definition already)
bool ComponentInstance::canConnect(Node* attachableObject)
{
    HardwareCluster* hardwareCluster = dynamic_cast<HardwareCluster*> (attachableObject);
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

    Component* component = dynamic_cast<Component*> (attachableObject);

    if(!hardwareNode && !hardwareCluster && !component){
        qWarning() << "ComponentInstance Node can only be connected to a HardwareNode, HardwareCluster or a Component";
        return false;
    }

    if(hardwareCluster || hardwareNode){
        //Check for deployment edges already.
        foreach(Edge* edge, getEdges(0)){
            if(edge->isDeploymentLink()){
                qWarning() << "ComponentInstance Node is already deployed!";
                return false;
            }
        }
    }

    if(component && getDefinition()){
        qWarning() << "ComponentInstance Node can only be connected to 1 Component Definition.";
        return false;
    }

    return Node::canConnect(attachableObject);
}
