#ifndef EDGE_H
#define EDGE_H

#include "graphml.h"
#include "graphmlcontainer.h"
#include <QString>

class GraphMLContainer;

//Base class of an edge object (Treated as Class for extended Node Types). Extends the GraphML Abstract Base Class.
class Edge: public GraphML{
    Q_OBJECT
public:
    //Constructor
    Edge(GraphMLContainer* source, GraphMLContainer* destination, QString name="");
    ~Edge();

    //Get the source graphml object of this Edge
    GraphMLContainer* getSource();

    //Get the destination graphml object of this Edge
    GraphMLContainer* getDestination();

    GraphMLContainer* getContainingGraph();

    //Return the graphml representation of this
    QString toGraphML(qint32 indentationLevel=0);


    QVector<GraphMLKey *> getKeys();

    bool contains(GraphMLContainer *item);
    QString toString();

signals:
    //Used to construct and tear down GUI elements.
    void constructGUI(Edge*);
    void destructGUI(Edge*, QString , QString, QString);


private:
    GraphMLContainer* source;
    GraphMLContainer* destination;

    static int _Eid;
};

#endif // EDGE_H
