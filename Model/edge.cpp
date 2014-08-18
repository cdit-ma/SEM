#include "edge.h"
#include "graphml.h"
#include "graphmldata.h"
#include <QDebug>

int Edge::_Eid = 0;


Edge::Edge(GraphMLContainer *source, GraphMLContainer *destination, QString name):GraphML(GraphML::EDGE, name)
{
    this->setID(QString("e%1").arg(this->_Eid++));

    //Set the instance Variables
    this->source = source;
    this->destination = destination;

    //Attach the Edge to its source/Destination
    this->getSource()->addEdge(this);
    this->getDestination()->addEdge(this);

}

Edge::~Edge()
{
    //Remove Edge!
    qDebug() << QString("Removed Edge[%1]!").arg(this->getID());

    getSource()->removeEdge(this);
    getDestination()->removeEdge(this);
};

GraphMLContainer *Edge::getSource() const
{
    return this->source;
};

GraphMLContainer *Edge::getDestination() const
{
    return this->destination;

}

GraphMLContainer *Edge::getContainingGraph()
{
    GraphMLContainer * source = this->getSource();
    GraphMLContainer * currentLookup = this->getDestination();

    while(currentLookup != 0){
        if(currentLookup->isAncestorOf(source)){
            return currentLookup;
        }else{
            currentLookup = currentLookup->getParent();
        }
    }
    return 0;

};

QString Edge::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<edge id=\"%1\" source=\"%2\" target =\"%3\"").arg(this->getID(),this->getSource()->getID(),this->getDestination()->getID());

    if(this->attachedData.size() > 0){
        returnable += ">\n";
        for(int i =0;i<this->attachedData.size();i++){
            returnable += this->attachedData[i]->toGraphML(indentationLevel + 1);
        }
        returnable += tabSpace + "</edge>\n";
    }else{
        returnable += "/>\n";
    }

    return returnable;
}

bool Edge::contains(GraphMLContainer *item)
{
   return item == this->source || item == this->destination;
}

QString Edge::toString()
{
    return QString("Edge[%1]: [" + this->getSource()->toString() +"] <-> [" + this->getDestination()->toString() + "]").arg(this->getID());
}
