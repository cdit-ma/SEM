#include "node.h"
#include <QDebug>
#include "graphmldata.h"

int Node::_Nid = 0;

Node::Node(QString name): GraphMLContainer(GraphML::NODE, name)
{
    this->setID(QString("n%1").arg(this->_Nid++));

    //Construct a Graph to hold the children of this Node type.
    this->childGraph = new Graph(name + ":");

    //Adopt the Graph, But using the Default adopt method
    GraphMLContainer::adopt(childGraph);

    //qDebug() << "Constructed Node[" << this->nodeKind <<"]: "<< this->getName();
}

Node::~Node(){
    qDebug() << "Destructing Node: " << this->getName();
}

QString Node::toGraphML(qint32 indentationLevel)
{
    QString tabSpace;
    for(int i=0;i<indentationLevel;i++){
        tabSpace += "\t";
    }

    QString returnable = tabSpace + QString("<node id =\"%1\">\n").arg(this->getID());

    for(int i=0; i < this->attachedData.size();i++){
        returnable += this->attachedData[i]->toGraphML(indentationLevel+1);
    }

    for(int i=0; i < this->descendants.size(); i++){

        returnable += this->descendants[i]->toGraphML(indentationLevel+1);
    }

    returnable += tabSpace + "</node>\n";
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
    if(this->childGraph != 0){
        this->childGraph->adopt(child);
    }
}

void Node::disown(GraphMLContainer *child)
{
    if(this->childGraph != 0){
        qDebug() << "Node Disown!";
        this->childGraph->disown(child);
    }
}
