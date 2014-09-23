#include "componentinstance.h"
#include <QDebug>
#include "eventport.h"
#include "inputeventport.h"
#include "outputeventport.h"
#include "../attribute.h"

ComponentInstance::ComponentInstance(QString name):Node(name)
{
    qDebug() << "Constructed ComponentInstance: "<< this->getName();
}

ComponentInstance::~ComponentInstance()
{
    //THIS IS A TEST
    //Destructor
}

bool ComponentInstance::isAdoptLegal(GraphMLContainer *attachableObject)
{

    EventPort* eventPort = dynamic_cast<EventPort*> (attachableObject);
    Attribute* attribute = dynamic_cast<Attribute*> (attachableObject);

    if(eventPort != 0 || attribute != 0){
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

bool ComponentInstance::isEdgeLegal(GraphMLContainer *attachableObject)
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


QString ComponentInstance::toString()
{
    return QString("ComponentInstance[%1]: "+this->getName()).arg(this->getID());
}

