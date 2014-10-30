#include "outeventportimpl.h"
#include <QDebug>
#include "../InterfaceDefinitions/outeventport.h"
#include "../InterfaceDefinitions//member.h"


OutEventPortImpl::OutEventPortImpl(QString name): Node(Node::NT_IMPL)
{
    //qDebug() << "Constructed InEventPort: "<< this->getName();
}

OutEventPortImpl::~OutEventPortImpl()
{

}


bool OutEventPortImpl::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

bool OutEventPortImpl::canConnect(Node* attachableObject)
{
    if(getDefinition()){
        return false;
    }
    OutEventPort* oep = dynamic_cast<OutEventPort*>(attachableObject);
    if(!oep){
        qCritical() << "Can Only connect an OutEventPortImpl to an OutEventPort";
        //return false;
    }

    return Node::canConnect(attachableObject);
}

QString OutEventPortImpl::toString()
{
    return QString("OutEventPortImpl[%1]: "+this->getName()).arg(this->getID());
}
