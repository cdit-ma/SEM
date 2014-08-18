#include "inputeventport.h"
#include <QDebug>
#include <typeinfo>
#include "outputeventport.h"

InputEventPort::InputEventPort(QString name):EventPort(name)
{
    qDebug() << "Constructed InputEventPort: "<< this->getName();
}

InputEventPort::~InputEventPort()
{

}

bool InputEventPort::isAdoptLegal(GraphMLContainer *child)
{
    return false;
}

bool InputEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    OutputEventPort* outputEventPort = dynamic_cast<OutputEventPort*> (attachableObject);

    if(outputEventPort == 0){
        return false;
    }

    if(this->getData("type") != attachableObject->getData("type")){
        qCritical() << "Cannot connect 2 different type names!";
        return false;
    }

    return true;
}


QString InputEventPort::toString()
{
    return QString("InputEventPort[%1]: "+this->getName()).arg(this->getID());

}
