#include "graphmlcontainer.h"
#include <QDebug>
GraphMLContainer::GraphMLContainer(GraphML::KIND kind, QString name):GraphML(kind, name)
{
    this->parent = 0;
}

GraphMLContainer::~GraphMLContainer()
{
    if(parent != 0){
        parent->disown(this);
    }

    removeEdges();
    removeChildren();

    //Kill the attached GUI element
    emit destructGUI(this);
}

void GraphMLContainer::setParent(GraphMLContainer *parent)
{
    if(this->parent != 0){
        //detach old parent
        this->parent->disown(this);
    }
    this->parent = parent;

    //Construct the GUI for this.
    emit constructGUI(this);
}

GraphMLContainer *GraphMLContainer::getParent()
{
    return this->parent;
}


Edge *GraphMLContainer::getEdge(GraphMLContainer *element)
{
    for (int i = 0; i < edges.size(); i++){
        if(this->edges[i]->contains(element)){
            return edges[i];
        }
    }
    return 0;
}

bool GraphMLContainer::isConnected(GraphMLContainer *element)
{
    if (getEdge(element) != 0){
        return true;
    }
    return false;
}

QVector<Edge *> GraphMLContainer::getEdges(int depth)
{
    QVector<Edge *> returnable;

    //Append this GraphMLContainers Edges which originate from here.
    for(int i=0; i < edges.size(); i++){
        if(edges[i]->getSource() == this){
            returnable.append(this->edges[i]);
        }
    }

    //If depth is not 0, keep Recursing.
    if(depth != 0){
        for(int i=0; i < descendants.size(); i++){
            QVector<Edge *> toAppend = descendants.at(i)->getEdges(depth -1);
            for(int j=0; j < toAppend.size(); j++){
                returnable.append(toAppend[j]);
            }
        }
    }

    return returnable;
}

void GraphMLContainer::removeEdges()
{
    //Delete all Edges
    while(!edges.isEmpty()){
        Edge* current = edges.first();
        edges.removeFirst();
        delete current;
    }
}

void GraphMLContainer::removeChildren()
{
    //Delete all Children
    while(!descendants.isEmpty()){
        GraphMLContainer* current = descendants.first();
        descendants.removeFirst();
        delete current;
    }
}


void GraphMLContainer::adopt(GraphMLContainer *child)
{
    //Construct the GUI for this.
    emit constructGUI(this);

    child->setParent(this);
    this->descendants.append(child);


}

void GraphMLContainer::disown(GraphMLContainer *child)
{
       if(isAncestorOf(child)){
        for (int i = 0; i < descendants.size(); i++){
            qDebug() << "\t" << descendants[i]->getName();
            if(descendants[i] == child){
                qCritical() << " Found Child. Removing!";
                descendants.removeAt(i);
                return;
            }
        }
        qCritical() <<  "Couldn't find child!";
    }
}

bool GraphMLContainer::isAncestorOf(GraphMLContainer *element)
{
    QVector<GraphMLContainer *> ancestors = this->getChildren(-1);
    return ancestors.contains(element);
    /*
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
    */
}

bool GraphMLContainer::isDescendantOf(GraphMLContainer *element)
{
    if(this->parent != 0){
        //Check if the parent is the element we are looking for;
        //Check if the element is a cousin.
        if(parent == element || parent->descendants.contains(element)){
            return true;
        }else{
            //Check up the tree.
            return this->parent->isDescendantOf(element);
        }
    }
    return false;

}

QVector<GraphMLContainer *> GraphMLContainer::getChildren(int depth)
{
    QVector<GraphMLContainer *> returnable;

    if(depth != 0){
        for(int i=0; i < this->descendants.size(); i++){
            returnable += this->descendants.at(i);
            returnable +=  this->descendants.at(i)->getChildren(depth - 1 );
        }
    }else{
        returnable += this->descendants;
    }
    return returnable;

}


QVector<GraphMLKey *> GraphMLContainer::getKeys(int depth)
{
    QVector<GraphMLContainer *> children = getChildren(depth);

    QVector<GraphMLKey *> allKeys;

    QVector<GraphMLData *> data = getData();
    for(int i=0; i < data.size(); i++){
        if(!allKeys.contains(data[i]->getKey())){
            allKeys.append(data[i]->getKey());
        }
    }

    for(int i=0; i < children.size(); i++){
        GraphMLContainer* child = children[i];

        QVector<GraphMLData *> data = child->getData();
        for(int j=0; j < data.size(); j++){
            if(!allKeys.contains(data[j]->getKey())){
                allKeys.append(data[j]->getKey());
            }
        }

    }

    return allKeys;

}


void GraphMLContainer::addEdge(Edge *edge)
{
    qDebug() << "Attached Edge to: " << getName() << " [" << getKind() << "]" << static_cast<void*>(this);
    qDebug()  << "\tAttached Edge: " << edge->getID();
    edges.append(edge);
}

void GraphMLContainer::removeEdge(Edge *edge)
{
    qCritical() << "Removing Edge from: " << getName() << " [" << getKind() << "]" << static_cast<void*>(this);
    qCritical() << "Looking for Edge: " << edge->getID();
    qCritical() << "Edges: " << edges.size();

    for (int i = 0; i < edges.size(); i++){
        qCritical() << "\t" << edges[i]->getID();
    }

    if(this->edges.contains(edge)){
        for (int i = 0; i < this->edges.size(); i++){
            if(this->edges[i] == edge){
                qCritical() << "Edge Removed!";
                edges.removeAt(i);
            }
        }
    }else{
        qCritical() <<  "Couldn't find Edge!";
    }


}
