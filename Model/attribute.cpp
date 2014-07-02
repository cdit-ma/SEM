#include "attribute.h"
#include <QDebug>

Attribute::Attribute(QString name): Node(name)
{
    qDebug() << "Constructed Attribute: "<< this->getName();
}

Attribute::~Attribute()
{
    //Destructor
}

//An attribute cannot adopt anything.
bool Attribute::isAdoptLegal(GraphMLContainer *child)
{
    return false;
}

bool Attribute::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return false;
}

QString Attribute::toString()
{
    return QString("Attribute[%1]: "+this->getName()).arg(this->getID());
}
