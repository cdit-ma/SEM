#include "InputEventPortIDL.h"
#include <QDebug>
#include <typeinfo>
#include "outputeventportidl.h"

InputEventPortIDL::InputEventPortIDL(QString name):EventPort(name)
{
    qDebug() << "Constructed InputEventPortIDL: "<< this->getName();
}

InputEventPortIDL::~InputEventPortIDL()
{

}

bool InputEventPortIDL::isAdoptLegal(GraphMLContainer *child)
{
    Q_UNUSED(child);
    return true;
}

bool InputEventPortIDL::isEdgeLegal(GraphMLContainer *attachableObject)
{
    OutputEventPort* outputEventPort = dynamic_cast<OutputEventPort*> (attachableObject);

    if(outputEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Output EVent Port";
        return false;
    }

    if(outputEventPort->getDataValue("type") != this->getDataValue("type")){
        qCritical() << "Cannot connect 2 Different IDL Types!";
        return false;
    }

    if(outputEventPort->getParent() == this->getParent()){
         //qCritical() << "Cannot connect 2 Ports from the same component!";
         //return false;
    }


    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;
}


QString InputEventPortIDL::toString()
{
    return QString("InputEventPortIDL[%1]: "+this->getName()).arg(this->getID());

}
