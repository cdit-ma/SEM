#include "outputeventport.h"
#include "inputeventport.h"
#include <QDebug>

OutputEventPort::OutputEventPort(QString name):EventPort(name)
{
    qDebug() << "Constructed OutputEventPort: "<< this->getName();
}

bool OutputEventPort::isAdoptLegal(GraphMLContainer *child)
{
    //Cannot adopt anything.
    return false;
}

bool OutputEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return false;
}

QString OutputEventPort::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());
    returnable += tabSpace + "\t" + QString("<data key =\"name\">%1</data>\n").arg(this->getName());
    returnable += tabSpace + "\t" + QString("<data key =\"type\">OutputEventPort</data>\n");
    returnable += Node::toGraphML(indentationLevel+1);
    returnable += tabSpace + "</node>\n";
    return returnable;
}

QString OutputEventPort::toString()
{
    return QString("OutputEventPort[%1]: "+this->getName()).arg(this->getID());
}
