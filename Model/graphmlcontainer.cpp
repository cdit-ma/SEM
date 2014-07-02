#include "graphmlcontainer.h"
#include <QDebug>
GraphMLContainer::GraphMLContainer(GraphML::KIND kind, QString name):GraphML(kind, name)
{
    this->parent = NULL;
}

GraphMLContainer::~GraphMLContainer()
{

}

void GraphMLContainer::setParent(GraphMLContainer *parent)
{
    if(this->parent != 0){
        //detach old parent
        this->parent->disown(this);
    }
    this->parent = parent;
}

GraphMLContainer *GraphMLContainer::getParent()
{
    return this->parent;
}

Edge *GraphMLContainer::getEdge(GraphMLContainer *element)
{
    for (int i = 0; i < this->edges.size(); i++){
        if(this->edges[i]->contains(element)){
            return edges[i];
        }
    }
    qCritical() << "Couldn't find edge!";
    return 0;
}

bool GraphMLContainer::isConnected(GraphMLContainer *element)
{

    for (int i = 0; i < this->edges.size(); i++){
        if(this->edges[i]->contains(element)){
            return true;
        }
    }
    return false;
}

QVector<Edge *> GraphMLContainer::getEdges() const
{
    return this->edges;
}

void GraphMLContainer::removeEdges()
{
    //Delete all Edges
    while(!this->edges.isEmpty()){
        delete this->edges.first();
    }
}

void GraphMLContainer::adopt(GraphMLContainer *child)
{
    qDebug() << this->toString() << " Adopting " << child->toString();

    child->setParent(this);

    this->descendants.append(child);
}

void GraphMLContainer::disown(GraphMLContainer *child)
{
    if(this->isDescendantOf(child)){
        for (int i = 0; i < this->descendants.size(); i++){
            if(this->descendants[i] == child){
                descendants.removeAt(i);
                return;
            }
        }
    }
    qCritical() <<  "Couldn't find child!";
}

bool GraphMLContainer::isAncestorOf(GraphMLContainer *element)
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

bool GraphMLContainer::isDescendantOf(GraphMLContainer *element)
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

QVector<GraphMLContainer *> GraphMLContainer::getChildren() const
{
    return this->descendants;
}

void GraphMLContainer::addEdge(Edge *edge)
{
    //qDebug() << this->getName() << " Attached Edge: " << edge->getID();
    this->edges.append(edge);
}

void GraphMLContainer::removeEdge(Edge *edge)
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
