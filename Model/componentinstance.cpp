#include "componentinstance.h"
#include <QDebug>
#include "eventport.h"
#include "inputeventport.h"
#include "outputeventport.h"

ComponentInstance::ComponentInstance(QString name):Node(name)
{
    qDebug() << "Constructed ComponentInstance: "<< this->getName();
}

ComponentInstance::~ComponentInstance()
{
    //Destructor
}

bool ComponentInstance::isAdoptLegal(GraphML *attachableObject)
{

    EventPort* eventPort = dynamic_cast<EventPort*> (attachableObject);

    if(eventPort != 0){
        //Do Event Port Specific things!
    }else{
        qWarning() << "ComponentInstance can only adopt an EventPort or an Attribute Node";;
        return false;
    }

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(attachableObject);
    }
    return true;
}

bool ComponentInstance::isEdgeLegal(GraphML *attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

    if(hardwareNode == 0){
        qWarning() << "ComponentInstance Node can only be connected to a HardwareNode";
        return false;
    }

    //Check for existing connection.
    if(this->isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
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

