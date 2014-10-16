#include "member.h"
#include <QDebug>

Member::Member(QString name):Node(name)
{
    //qDebug() << "Constructed Member: "<< this->getName();

}

Member::~Member()
{
}

QString Member::toString()
{
    return QString("Member[%1]: "+this->getName()).arg(this->getID());

}

bool Member::isEdgeLegal(GraphMLContainer *attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;

}

bool Member::isAdoptLegal(GraphMLContainer *child)
{
    Q_UNUSED(child);
    return false;
}
