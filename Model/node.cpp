#include "node.h"
#include <QDebug>
#include "graphmldata.h"
Node::Node(QString name): GraphMLContainer(GraphML::NODE, name)
{
    //Construct a Graph to hold the children of this Node type.
    this->childGraph = new Graph(name + ":");

    //Adopt the Graph, But using the Default adopt method
    GraphMLContainer::adopt(childGraph);

    //qDebug() << "Constructed Node[" << this->nodeKind <<"]: "<< this->getName();
}

Node::~Node(){
    qDebug() << "Destructing Node";
}

QString Node::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }
    QString returnable;

    for(int i=0; i <this->attachedData.size();i++){
        returnable += this->attachedData[i]->toGraphML(indentationLevel);
    }

    for(int i=0; i < this->descendants.size(); i++){

        returnable += this->descendants[i]->toGraphML(indentationLevel);
    }

    for(int i=0; i < this->edges.size(); i++){
        //Only store edges which originate from here.
        if(this->edges[i]->getSource() == this){
            returnable += this->edges[i]->toGraphML(indentationLevel);
        }
    }
    return returnable;
}


QString Node::toString()
{
    return QString("Node[%1]: "+this->getName()).arg(this->getID());
}

Graph *Node::getGraph()
{
    return this->childGraph;

}



void Node::adopt(GraphMLContainer *child)
{

    if(this->childGraph != NULL){
        this->childGraph->adopt(child);
    }
}

void Node::disown(GraphMLContainer *child)
{
    if(this->childGraph != NULL){
        this->childGraph->disown(child);
    }
}
