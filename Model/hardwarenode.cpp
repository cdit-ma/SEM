#include "hardwarenode.h"
#include <QDebug>
#include "componentinstance.h"
#include "assembly.h"

HardwareNode::HardwareNode(QString name):Node(this->nodeKind,name)
{
    qDebug() << "Constructed HardwareNode: "<< this->getName();
}

HardwareNode::~HardwareNode()
{
    //Destructor.
}

bool HardwareNode::isAdoptLegal(GraphML *child)
{
    //Specific stuff here!

    //Return only on False statements.
    //Do RETURN FALSE

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(child);
    }
    return true;
}

bool HardwareNode::isEdgeLegal(GraphML *attachableObject)
{
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (attachableObject);
    Assembly* assemblyInstance = dynamic_cast<Assembly*> (attachableObject);

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

QString HardwareNode::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }
    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());
    returnable += tabSpace + "\t" + QString("<data key =\"name\">%1</data>\n").arg(this->getName());
    returnable += tabSpace + "\t" + QString("<data key =\"type\">HardwareNode</data>\n");
    returnable += Node::toGraphML(indentationLevel+1);
    returnable += tabSpace + "</node>\n";
    return returnable;
}

QString HardwareNode::toString()
{
    return QString("HardwareNode[%1]: "+this->getName()).arg(this->getID());
}
