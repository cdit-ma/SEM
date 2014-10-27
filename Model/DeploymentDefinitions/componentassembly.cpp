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
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

     //Is this child a HardwareNode?
    if(hardwareNode == 0){
        //Check stuff!
        qWarning() << "AssemblyNode can only connect to a HardwareNode";
        return false;
    }

    //Check for existing connection.
    if(this->isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return true;
}

QString ComponentAssembly::toString()
{
    return QString("Assembly[%1]: "+this->getName()).arg(this->getID());
}
