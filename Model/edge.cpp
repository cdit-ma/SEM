#include "edge.h"
#include "graphml.h"
#include <QDebug>

//The Unique ID Counter
qint32 Edge::idCounter = 0;

Edge::Edge(GraphML *source, GraphML *destination)
{
    //Set the instance Variables
    this->source = source;
    this->destination = destination;
    this->id=idCounter;

    //Attach the Edge to its source/Destination
    this->getSource()->addEdge(this);
    this->getDestination()->addEdge(this);

    qDebug() << this->toString();

    //Increment idCounter
    this->idCounter++;
}

Edge::~Edge()
{
    //Remove Edge!
    this->getSource()->removeEdge(this);
    this->getDestination()->removeEdge(this);
    qDebug() << QString("Removed Edge[%1]!").arg(this->getID());
};

GraphML *Edge::getSource() const
{
    return this->source;
};

GraphML *Edge::getDestination() const
{
    return this->destination;

};

QString Edge::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<edge id=\"" + QString::number(this->getID()) + "\" source=\""+QString::number(this->getSource()->getID())+"\" target=\""+QString::number(this->getDestination()->getID())+"\" />\n");
    return returnable;
}

qint32 Edge::getID() const
{
    return this->id;
}

QString Edge::toString()
{
    return QString("Edge[%1]: [" + this->getSource()->toString() +"] <-> [" + this->getDestination()->toString() + "]").arg(this->getID());
}
