#include "member.h"
#include <QDebug>

Member::Member(QString name):Node()
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

bool Member::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;

}

bool Member::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
