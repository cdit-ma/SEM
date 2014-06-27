#include "outputeventport.h"

OutputEventPort::OutputEventPort(QString name):EventPort(this->portKind,name)
{
    qDebug() << "Constructed OutputEventPort: "<< this->getName();
}

bool OutputEventPort::isAdoptLegal(GraphML *child)
{
    return false;
}

bool OutputEventPort::isEdgeLegal(GraphML *attachableObject)
{
    return false;

}

QString OutputEventPort::toGraphML(qint32 indentationLevel)
{
    return QString("");

}

QString OutputEventPort::toString()
{
    return QString("");

}
