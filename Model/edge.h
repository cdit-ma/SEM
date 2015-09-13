#ifndef EDGE_H
#define EDGE_H

#include "graphml.h"
#include <QString>

class Node;

//Base class of an edge object (Treated as Class for extended Node Types). Extends the GraphML Abstract Base Class.
class Edge: public GraphML{
    Q_OBJECT
public:
    //Constructor
    Edge(Node* source, Node* destination, QString name="");
    ~Edge();

    //Get the source graphml object of this Edge
    Node* getSource();

    //Get the destination graphml object of this Edge
    Node* getDestination();

    Node* getContainingGraph();

    //Return the graphml representation of this
    QString toGraphML(qint32 indentationLevel=0);


    QVector<GraphMLKey *> getKeys();

    bool isInstanceToInstanceLink();
    bool isInstanceLink();
    bool isImplLink();
    bool isAggregateLink();
    bool isDeploymentLink();
    bool isAssemblyLink();
    bool isComponentLink();


    bool isDelegateLink();
    bool contains(Node *item);
    QString toString();


private:
    Node* source;
    Node* destination;

    static int _Eid;
};

#endif // EDGE_H
