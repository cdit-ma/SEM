#include "edge.h"
#include "graphml.h"
#include "graphmldata.h"
#include <QDebug>

int Edge::_Eid = 0;


Edge::Edge(GraphMLContainer *s, GraphMLContainer *d, QString name):GraphML(GraphML::EDGE, name)
{
    setID(QString("e%1").arg(this->_Eid++));


    //Set the instance Variables
    source = s;

    destination = d;

    //Attach the Edge to its source/Destination
    source->addEdge(this);
    destination->addEdge(this);
}

Edge::~Edge()
{
    emit destructGUI(this);

    //Remove Edge!
    destination->removeEdge(this);
    source->removeEdge(this);

    qCritical() << QString("Removed Edge[%1]!").arg(this->getID());
}

GraphMLContainer *Edge::getSource()
{
    return this->source;
}

GraphMLContainer *Edge::getDestination()
{
    return this->destination;

}

GraphMLContainer *Edge::getContainingGraph()
{
    GraphMLContainer* source = this->getSource();
    GraphMLContainer* currentLookup = this->getDestination();

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
    tabSpace.fill('\t', indentationLevel);

    QString returnable = tabSpace + QString("<edge id=\"%1\" source=\"%2\" target =\"%3\"").arg(this->getID(),this->getSource()->getID(),this->getDestination()->getID());

    if(attachedData.size() > 0){
        returnable += ">\n";
        for(int i =0;i<attachedData.size();i++){
            returnable += attachedData[i]->toGraphML(indentationLevel + 1);
        }
        returnable += tabSpace + "</edge>\n";
    }else{
        returnable += "/>\n";
    }

    return returnable;
}

QVector<GraphMLKey *> Edge::getKeys()
{
    QVector<GraphMLKey *> keys;
    foreach(GraphMLData* data, attachedData){
        GraphMLKey* key = data->getKey();
        if(!keys.contains(key)){
            keys.append(key);
        }
    }
    return keys;
}

bool Edge::contains(GraphMLContainer *item)
{
    return item == this->source || item == this->destination;
}

QString Edge::toString()
{
    return QString("Edge[%1]: [" + this->getSource()->toString() +"] <-> [" + this->getDestination()->toString() + "]").arg(this->getID());
}
