#include "inputeventport.h"
#include <QDebug>
#include <typeinfo>
#include "outputeventport.h"

InputEventPort::InputEventPort(QString name):EventPort(name)
{
    qDebug() << "Constructed InputEventPort: "<< this->getName();
}

bool InputEventPort::isAdoptLegal(GraphMLContainer *child)
{
    return false;
}

bool InputEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{

    OutputEventPort* outputEventPort = dynamic_cast<OutputEventPort*> (attachableObject);

    if(outputEventPort != 0){
    }else{
        return false;
    }

    return true;
}

QString InputEventPort::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());
    returnable += tabSpace + "\t" + QString("<data key =\"name\">%1</data>\n").arg(this->getName());
    returnable += tabSpace + "\t" + QString("<data key =\"type\">InputEventPort</data>\n");
    returnable += Node::toGraphML(indentationLevel+1);
    returnable += tabSpace + "</node>\n";
    return returnable;

}

QString InputEventPort::toString()
{
    return QString("InputEventPort[%1]: "+this->getName()).arg(this->getID());

}
