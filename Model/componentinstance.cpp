#include "componentinstance.h"
#include <QDebug>

ComponentInstance::ComponentInstance(QString name):Node(this->nodeKind,name)
{
    qDebug() << "Constructed ComponentInstance: "<< this->getName();
}

ComponentInstance::~ComponentInstance()
{
    //Destructor
}

bool ComponentInstance::isAdoptLegal(GraphML *child)
{
    //Specific stuff here!

    //Return only on False statements.
    //Do RETURN FALSE

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(child);
    }
    return true;
}

bool ComponentInstance::isEdgeLegal(GraphML *attachableObject)
{
    //Check for existing connection.
    if(this->isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    //If its a node, cast as a node, and check sub type.
    if(attachableObject->getKind() != Node::classKind){
        qWarning() << "ComponentInstance Node can only be connected to another Node Type";
        return false;
    }else{
        if(((Node *)attachableObject)->getNodeKind() != HardwareNode::nodeKind){
            qWarning() << "ComponentInstance Node can only be connected to a HardwareNode";
            return false;
        }
    }

    return true;
}

QString ComponentInstance::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());
    returnable += tabSpace + "\t" + QString("<data key =\"name\">%1</data>\n").arg(this->getName());
    returnable += tabSpace + "\t" + QString("<data key =\"type\">ComponentInstance</data>\n");
    returnable += Node::toGraphML(indentationLevel+1);
    returnable += tabSpace + "</node>\n";
    return returnable;
}

QString ComponentInstance::toString()
{
    return QString("ComponentInstance[%1]: "+this->getName()).arg(this->getID());
}

