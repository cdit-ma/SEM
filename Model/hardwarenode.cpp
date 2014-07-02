#include "hardwarenode.h"
#include <QDebug>
#include "componentinstance.h"
#include "componentassembly.h"

HardwareNode::HardwareNode(QString name):Node(name)
{
    qDebug() << "Constructed HardwareNode: "<< this->getName();
}

HardwareNode::~HardwareNode()
{
    //Destructor.
}

bool HardwareNode::isAdoptLegal(GraphMLContainer *child)
{
    //Specific stuff here!

    //Return only on False statements.
    //Do RETURN FALSE

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(child);
    }
    return true;
}

bool HardwareNode::isEdgeLegal(GraphMLContainer *attachableObject)
{
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (attachableObject);
    ComponentAssembly* assemblyInstance = dynamic_cast<ComponentAssembly*> (attachableObject);

    if(componentInstance != 0){
        //Do Component Instance Checks
    }else if(assemblyInstance != 0){
        //Do Assembly Checks
    }else{
        qWarning() << "HardwareNode can only be connected to a Component Instance or Assembly Node.";
        return false;
    }

    //Check for existing connection.
    if(this->isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return true;
}


QString HardwareNode::toString()
{
    return QString("HardwareNode[%1]: "+this->getName()).arg(this->getID());
}
