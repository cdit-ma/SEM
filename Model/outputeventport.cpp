#include "outputeventport.h"
#include "inputeventport.h"
#include <QDebug>

OutputEventPort::OutputEventPort(QString name):EventPort(name)
{
    qDebug() << "Constructed OutputEventPort: "<< this->getName();
}

OutputEventPort::~OutputEventPort()
{

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

    if(inputEventPort->getDataValue("type") != this->getDataValue("type")){
        qCritical() << "Cannot connect 2 Different IDL Types!";
        return false;
    }

    if(inputEventPort->getParent() == this->getParent()){
         //qCritical() << "Cannot connect 2 Ports from the same component!";
         //return false;
    }

    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;

}

QString OutputEventPort::toString()
{
    return QString("OutputEventPort[%1]: "+this->getName()).arg(this->getID());
}
