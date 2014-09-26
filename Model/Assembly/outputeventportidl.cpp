#include "OutputEventPortIDL.h"
#include "inputeventport.h"
#include <QDebug>

OutputEventPortIDL::OutputEventPortIDL(QString name):EventPort(name)
{
    qDebug() << "Constructed OutputEventPortIDL: "<< this->getName();
}

OutputEventPortIDL::~OutputEventPortIDL()
{

}

bool OutputEventPortIDL::isAdoptLegal(GraphMLContainer *child)
{
    Q_UNUSED(child);
    //Cannot adopt anything.
    return true;
}

bool OutputEventPortIDL::isEdgeLegal(GraphMLContainer *attachableObject)
{
    InputEventPortIDL* inputEventPort = dynamic_cast<InputEventPortIDL*> (attachableObject);

    //Check Event Name (TYPE)
    if(inputEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Input Event Port";
        return false;
    }

    if(inputEventPort->getDataValue("type") != this->getDataValue("type")){
        qCritical() << "Cannot connect 2 Different IDL Types!";
        return false;
    }

    if(inputEventPort->getParent() == this->getParent()){
         qCritical() << "Cannot connect 2 Ports from the same component!";
         //return false;
    }

    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;

}

QString OutputEventPortIDL::toString()
{
    return QString("OutputEventPortIDL[%1]: "+this->getName()).arg(this->getID());
}
