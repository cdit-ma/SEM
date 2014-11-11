#include "attributeimpl.h"
#include "../InterfaceDefinitions/attribute.h"

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

    return false;
}

bool AttributeImpl::canAdoptChild(Node *child)
{
    return true;
}

