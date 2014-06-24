#include "hardwarenode.h"
#include <QDebug>

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
    if(attachableObject->getKind() == this->getKind()){
        return true;
    }
    return false;
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
