#include "attributeimpl.h"
#include "../InterfaceDefinitions/attribute.h"
#include <QDebug>

AttributeImpl::AttributeImpl(QString name):Node(Node::NT_IMPL)
{
}

AttributeImpl::~AttributeImpl()
{

}


QString AttributeImpl::toString()
{
    return QString("AttributeImpl[%1]: "+this->getName()).arg(this->getID());
}

bool AttributeImpl::canConnect(Node* attachableObject)
{
    Attribute* attribute = dynamic_cast<Attribute*>(attachableObject);

    if(getDefinition()){
        qWarning() << "AttributeImpl already has a definition already";
        return false;
    }

    if(!attribute){
        qWarning() << "AttributeImpl can only connect to an Attribute.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool AttributeImpl::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

