#ifndef EDGE_H
#define EDGE_H
#include <QString>

//Forward Definition
class GraphML;

//Base class of an edge object (Treated as Class for extended Node Types). Extends the GraphML Abstract Base Class.
class Edge{
public:
    //Constructor
    Edge(GraphML* source, GraphML* destination);
    ~Edge();

    //Get the source graphml object of this Edge
    GraphML *getSource() const;

    //Get the destination graphml object of this Edge
    GraphML *getDestination() const;

    //Return the graphml representation of this
    QString toGraphML(qint32 indentationLevel=0);

    //Get the Unique ID of this object
    qint32 getID() const;

    QString toString();

protected:
    GraphML *source;
    GraphML *destination;
    qint32 id;
    QString name;
    //The unique Edge ID counter;
    static qint32 idCounter;
};

#endif // EDGE_H
