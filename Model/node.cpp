#include "node.h"
#include <QDebug>
#include "graphmldata.h"
Node::Node(QString name): GraphML(this->classKind ,name)
{
    //Set the Node kind to that of what is constructed.
    this->nodeKind = 0;

    //Construct a Graph to hold the children of this Node type.
    this->childGraph = new Graph(name + ":");

    //Adopt the Graph, But using the Default adopt method
    GraphML::adopt(childGraph);

    qDebug() << "Constructed Node[" << this->nodeKind <<"]: "<< this->getName();
}

Node::~Node(){
    qDebug() << "Destructing Node";
    //DESTRUCT
}

QString Node::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }
    QString returnable;

    for(int i=0; i <this->containedData.size();i++){
        returnable += this->containedData[i]->toGraphML(indentationLevel);
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

qint32 Node::getNodeKind() const
{
    return this->nodeKind;
}


void Node::adopt(GraphML *child)
{
    if(this->childGraph != NULL){
        this->childGraph->adopt(child);
    }
}

void Node::disown(GraphML *child)
{
    if(this->childGraph != NULL){
        this->childGraph->disown(child);
    }
}
