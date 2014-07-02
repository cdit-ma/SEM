#include "outputeventport.h"
#include "inputeventport.h"
#include <QDebug>

OutputEventPort::OutputEventPort(QString name):EventPort(name)
{
    qDebug() << "Constructed OutputEventPort: "<< this->getName();
}

bool OutputEventPort::isAdoptLegal(GraphMLContainer *child)
{
    //Cannot adopt anything.
    return false;
}

bool OutputEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    InputEventPort* inputEventPort = dynamic_cast<InputEventPort*> (attachableObject);


    //Check Event Name (TYPE)
    if(inputEventPort == 0){
        return false;
    }

    if(this->getData("type") != attachableObject->getData("type")){
        qCritical() << "Cannot connect 2 different type names!";
        return false;
    }

    return true;

}

QString OutputEventPort::toString()
{
    return QString("OutputEventPort[%1]: "+this->getName()).arg(this->getID());
}
