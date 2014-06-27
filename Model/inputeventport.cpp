#include "inputeventport.h"


InputEventPort::InputEventPort(QString name):EventPort(this->portKind,name)
{
    qDebug() << "Constructed InputEventPort: "<< this->getName();
}

bool InputEventPort::isAdoptLegal(GraphML *child)
{
    return false;
}

bool InputEventPort::isEdgeLegal(GraphML *attachableObject)
{
    return false;

}

QString InputEventPort::toGraphML(qint32 indentationLevel)
{
    return QString("");

}

QString InputEventPort::toString()
{
    return QString("");

}
