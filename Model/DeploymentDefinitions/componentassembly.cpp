#include "componentassembly.h"
#include "componentinstance.h"
#include "hardwarecluster.h"
#include "hardwarenode.h"
#include <QDebug>
ComponentAssembly::ComponentAssembly(QString name):Node()
{
    //qDebug() << "Constructed Assembly: "<< this->getName();
}

ComponentAssembly::~ComponentAssembly()
{
    //Destructor
}

bool ComponentAssembly::canAdoptChild(Node *child)
{
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (child);

    //Is this child a ComponentInstance?
    if(!componentInstance){
        qWarning() << "Assembly Node can only adopt a Component Instance Node";
        return false;
    }

    return Node::canAdoptChild(child);
}

bool ComponentAssembly::canConnect(Node* attachableObject)
{

    HardwareCluster* hardwareCluster = dynamic_cast<HardwareCluster*> (attachableObject);
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

    if(!hardwareNode && !hardwareCluster){
        qWarning() << "ComponentAssembly Node can only be connected to a HardwareNode or HardwareCluster.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

QString ComponentAssembly::toString()
{
    return QString("Assembly[%1]: "+this->getName()).arg(this->getID());
}
