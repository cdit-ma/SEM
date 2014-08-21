#include "componentassembly.h"
#include "componentinstance.h"
#include <QDebug>
ComponentAssembly::ComponentAssembly(QString name):Node(name)
{
    qDebug() << "Constructed Assembly: "<< this->getName();
}

ComponentAssembly::~ComponentAssembly()
{
    //Destructor
}

bool ComponentAssembly::isAdoptLegal(GraphMLContainer *child)
{
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (child);

    //Is this child a ComponentInstance?
    if(componentInstance == 0){
        qWarning() << "Assembly Node can only adopt a Component Instance Node";
        return false;
    }

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(child);
    }
    return true;
}

bool ComponentAssembly::isEdgeLegal(GraphMLContainer *attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

     //Is this child a HardwareNode?
    if(hardwareNode == 0){
        //Check stuff!
        qWarning() << "AssemblyNode can only connect to a HardwareNode";
        //return false;
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
