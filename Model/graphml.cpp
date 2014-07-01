#include "graphml.h"
#include <QDebug>

qint32 GraphML::idCounter = 0;

GraphML::GraphML(GraphML::KIND kind, QString name)
{
    //Set variables
    this->id = idCounter;
    this->kind = kind;

    this->name=name;

    this->parent = NULL;

    //Increment Counter
    this->idCounter++;
    //qDebug() << "Constructed: GraphML[" << this->getID() << ":" << this->kind << "] = " << this->getName();
}



void GraphML::setParent(GraphML *parent)
{
    if(this->parent != NULL){
        //detach old parent
        this->parent->disown(this);

    }

    this->parent = parent;
}

GraphML *GraphML::getParent()
{
    return this->parent;
}

GraphML::~GraphML()
{

    //Delete Object
    qDebug() << "Destructing GraphML";
}

QString GraphML::getName() const
{
    return this->name;
}

QVector<GraphMLData *> GraphML::getData()
{
    return this->containedData;
}

void GraphML::attachData(GraphMLData *data)
{
    this->containedData.append(data);
}

void GraphML::attachData(QVector<GraphMLData *> data)
{
    while(!data.isEmpty()){
        this->attachData(data.front());
        data.pop_front();
    }
}

void GraphML::setName(QString name)
{
    this->name = name;
}

qint32 GraphML::getKind() const
{
    return this->kind;
}

qint32 GraphML::getID() const
{
    return this->id;
}

void GraphML::setID(qint32 id)
{
    this->id = id;
}

void GraphML::addEdge(Edge *edge)
{

    qDebug() << this->getName() << " Attached Edge: " << edge->getID();
    this->edges.append(edge);
}

void GraphML::removeEdge(Edge *edge)
{
    if(this->edges.contains(edge)){
        for (int i = 0; i < this->edges.size(); i++){
            if(this->edges[i] == edge){
                edges.removeAt(i);
            }
        }
        qCritical() <<  "Couldn't find Edge!";
    }
}

void GraphML::removeEdges()
{
    //Delete all Edges
    while(!this->edges.isEmpty()){
        delete this->edges.first();
    }
}

void GraphML::adopt(GraphML *child)
{
    qDebug() << this->toString() << " Adopting " << child->toString();

    child->setParent(this);
    this->descendants.append(child);
}

void GraphML::disown(GraphML *child)
{
    if(this->descendants.contains(child)){
        for (int i = 0; i < this->descendants.size(); i++){
            if(this->descendants[i] == child){
                descendants.removeAt(i);
                return;
            }
        }

        qCritical() <<  "Couldn't find child!";
    }
}

bool GraphML::isAncestorOf(GraphML *element)
{
    //Check for an immediate descendant
    if(this->descendants.contains(element)){
        return true;
    }else{
        //Check for a descendants descendant.
        for(int i=0; i < this->descendants.size();i++){
            return descendants[i]->isAncestorOf(element);
        }
    }
    return false;
}

bool GraphML::isDescendantOf(GraphML *element)
{

    if(this->parent != NULL){
        //Check if the parent is the element we are looking for;
        //Check if we element is a cousin.
        if(this->parent == element || this->parent->descendants.contains(element)){
            return true;
        }else{
            //Check up the tree.
            return this->parent->isDescendantOf(element);
        }
    }
    return false;
}

QVector<GraphML *> GraphML::getChildren() const
{
    return this->descendants;
}

QVector<Edge *> GraphML::getEdges() const
{
    return this->edges;
}

bool GraphML::isConnected(GraphML *element)
{
    QVector<Edge *> connectedEdges = this->getEdges();
    for (int i = 0; i < connectedEdges.size(); i++){
        if(connectedEdges[i]->getDestination() == element){
            return true;
        }
        if(connectedEdges[i]->getSource() == element){
            return true;
        }
    }
    return false;
}

Edge *GraphML::getEdge(GraphML *element)
{
    QVector<Edge *> connectedEdges = this->getEdges();
    for (int i = 0; i < connectedEdges.size(); i++){
        if(connectedEdges[i]->getDestination() == element){
            return connectedEdges[i];
        }
        if(connectedEdges[i]->getSource() == element){
            return connectedEdges[i];
        }
    }
    qCritical() <<  "Couldn't find Edge!";
    return NULL;
}
