#include "outeventportdelegate.h"
#include "ineventportdelegate.h"
#include "ineventportinstance.h"
#include "outeventportinstance.h"
#include <QDebug>
#include <typeinfo>

OutEventPortDelegate::OutEventPortDelegate():Node(Node::NT_NODE)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

OutEventPortDelegate::~OutEventPortDelegate()
{
}


bool OutEventPortDelegate::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

bool OutEventPortDelegate::canConnect(Node* attachableObject)
{

    OutEventPortInstance* outEventPortInstance = dynamic_cast<OutEventPortInstance*>(attachableObject);

    if(inEventPortInstance == 0){
        qCritical() << "Cannot connect an IEPD to anything which isn't an EventPort instance";
        return false;
    }

    return Node::canConnect(attachableObject);
}


QString OutEventPortDelegate::toString()
{
    return QString("InEventPortDelegate[%1]: " + this->getName()).arg(this->getID());

}
