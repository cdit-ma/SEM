#ifndef GRAPHML_H
#define GRAPHML_H

#include "edge.h"


//Forward Definition
class GraphMLData;

#include <QVector>
#include <QString>

//Abstract Base class for the Graphml objects in a graph. Extended by Graph and Node
//Some pure Virtual methods. Cannot be directly Instantiated.
class GraphML{
    //Friend to Edge, meaning Edge can access the private members of this class!
    friend class Edge;

public:
    enum KIND {NONE, GRAPH, NODE, EDGE};


    //Super class constructor; Relative ID and name provided. Used only to be called by the extended classes.
    GraphML(KIND kind, QString name);



    void setParent(GraphML *parent);

    GraphML* getParent();

    //The destructor.
    ~GraphML();

    //Get the name of this graphml object
    QString getName() const;


    //Get a list of Data objects contained by this GraphML
    QVector<GraphMLData *> getData();

    //Attach a data object contained by this GraphML
    void attachData(GraphMLData* data);
    void attachData(QVector<GraphMLData* > data);

    //Set the name of this graphml object
    void setName(QString name);

    //Get the kind of this graphml object
    qint32 getKind() const;

    //Get the Unique ID of this object
    qint32 getID() const;

    //Set the ID to be that provided plus the offset of the total number of graphml object
    void setID(qint32 id);

    //Removes all edges.
    void removeEdges();

    //Add a child to this graphml object.
    virtual void adopt(GraphML *child);

    //Remove a child from this graphml object.
    virtual void disown(GraphML *child);

    //Checks if the element is a parent, or a relative of a parent.
    bool isAncestorOf(GraphML *element);

    //Checks if the element is a child, or a relative of a child.
    bool isDescendantOf(GraphML *element);

    //Get a list of graphml objects contained by this graph
    QVector<GraphML *> getChildren() const;

    //Get a list of edges which is connected to this graphml object.
    QVector<Edge *> getEdges() const;

    //Checks to see if there is an edge element connecting to this.
    bool isConnected(GraphML *element);

    //Gets the edge that is connecting the element to this.
    Edge* getEdge(GraphML *element);

    //Can this graph adopt this graph.
    //Pure Virtual => Must be overwritten by subclasses!
    virtual bool isAdoptLegal(GraphML *child)=0;

    //Can a Edge be constructed between this graphml and the provided graphml object.
    //Pure Virtual => Must be overwritten by subclasses!
    virtual bool isEdgeLegal(GraphML *attachableObject)=0;

    //Returns a string graphml representation of this graphml object
    //Pure Virtual => Must be overwritten by subclasses!
    virtual QString toGraphML(qint32 indentationLevel=0)=0;

    //Returns a string representation of this graphml object.
    virtual QString toString()=0;

    //The unique ID counter;
    static qint32 idCounter;

    //The unique ID of this class.
    const static qint32 classKind = 0;
protected:
    //The list of contained children GraphML elements. (Top level only)
    QVector<GraphML *> descendants;

    QVector<GraphMLData *> containedData;
    //The list of contained Edge elements in this graph. (Top level only)
    QVector<Edge *> edges;

private:
    //Add an edge which is connected to this graphml object.
    void addEdge(Edge *edge);

    //Remove an edge which is connected to this graphml object.
    void removeEdge(Edge *edge);

    //The name of this graphml object.
    QString name;

    //GraphML
    GraphML* parent;

    //The unique ID of this graphml object.
    qint32 id;

    //The unique kind ID of this graph object.
    KIND kind;

};


#endif // GRAPHML_H


