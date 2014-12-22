#include "ineventportdelegate.h"
#include "outeventportdelegate.h"
#include "ineventportinstance.h"
#include "outeventportinstance.h"
#include <QDebug>
#include <typeinfo>

InEventPortDelegate::InEventPortDelegate():Node(Node::NT_NODE)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

InEventPortDelegate::~InEventPortDelegate()
{
}


bool InEventPortDelegate::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

bool InEventPortDelegate::canConnect(Node* attachableObject)
{

    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(attachableObject);

    if(inEventPortInstance == 0){
        qCritical() << "Cannot connect an IEPD to anything which isn't an EventPort instance";
        return false;
    }

    return Node::canConnect(attachableObject);
}


QString InEventPortDelegate::toString()
{
    return QString("InEventPortDelegate[%1]: " + this->getName()).arg(this->getID());

}


