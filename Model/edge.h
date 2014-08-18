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
    GraphMLContainer *getSource() const;

    //Get the destination graphml object of this Edge
    GraphMLContainer *getDestination() const;

    GraphMLContainer *getContainingGraph();

    //Return the graphml representation of this
    QString toGraphML(qint32 indentationLevel=0);

    bool contains(GraphMLContainer *item);
    QString toString();
private:
    GraphMLContainer* source;
    GraphMLContainer* destination;

    static int _Eid;
};

#endif // EDGE_H
