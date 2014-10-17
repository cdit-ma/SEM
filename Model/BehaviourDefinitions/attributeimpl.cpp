#include "attributeimpl.h"
#include "../InterfaceDefinitions/attribute.h"

AttributeImpl::AttributeImpl(QString name):Node(name)
{
    def = 0;
}

AttributeImpl::~AttributeImpl()
{

}

void AttributeImpl::setDefinition(Attribute *def)
{
    this->def = def;
}

Attribute *AttributeImpl::getDefinition()
{
    return def;
}

QString AttributeImpl::toString()
{
    return QString("AttributeImpl[%1]: "+this->getName()).arg(this->getID());
}

bool AttributeImpl::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

bool AttributeImpl::isAdoptLegal(GraphMLContainer *child)
{
    return true;
}

