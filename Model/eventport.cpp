#include "eventport.h"
#include <QDebug>

EventPort::EventPort(qint32 portType, QString name):Node(this->nodeKind,name)
{
    qDebug() << "Constructed EventPort: "<< this->getName();
    this->portKind = portType;
}

EventPort::~EventPort()
{
    //Destructor
}

QString EventPort::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());
    returnable += tabSpace + "\t" + QString("<data key =\"name\">%1</data>\n").arg(this->getName());
    returnable += tabSpace + "\t" + QString("<data key =\"type\">EventPort</data>\n");
    returnable += Node::toGraphML(indentationLevel+1);
    returnable += tabSpace + "</node>\n";
    return returnable;
}

QString EventPort::toString()
{
    return QString("EventPort[%1]: "+this->getName()).arg(this->getID());
}
