#include "graphml.h"

#ifndef GRAPHMLCONTAINER_H
#define GRAPHMLCONTAINER_H

#include "edge.h"

class GraphMLContainer : public GraphML
{
    //Friend to Edge, meaning Edge can access the private members of this class!
    friend class Edge;
public:
    //Constructor
    GraphMLContainer(GraphML::KIND kind, QString name);

    //Destructor.
    ~GraphMLContainer();

    //Sets the Parent of this Container
    void setParent(GraphMLContainer *parent);

    //Gets the Parent
    GraphMLContainer* getParent();

    /*                      */
    /*EDGE FUNCTIONS        */
    /*                      */

    //Can a Edge be constructed between this graphml and the provided graphml object.
    //Pure Virtual => Must be overwritten by subclasses!
    virtual bool isEdgeLegal(GraphMLContainer *attachableObject)=0;

    //Gets the edge that is connecting the element to this.
    Edge* getEdge(GraphMLContainer *element);

    //Checks to see if there is an edge element connecting to this.
    bool isConnected(GraphMLContainer *element);

    //Get a list of edges which is connected to this graphml object.
    QVector<Edge *> getEdges() const;

    //Removes all edges.
    void removeEdges();

    /*                      */
    /* CHILDREN FUNCTIONS   */
    /*                      */

    //Add a child to this graphml object.
    virtual void adopt(GraphMLContainer *child);

    //Remove a child from this graphml object.
    virtual void disown(GraphMLContainer *child);

    //Checks if the element is a parent, or a relative of a parent.
    bool isAncestorOf(GraphMLContainer *element);

    //Checks if the element is a child, or a relative of a child.
    bool isDescendantOf(GraphMLContainer *element);

    //Get a list of graphml objects contained by this graph
    QVector<GraphMLContainer *> getChildren() const;

    //Can this graph adopt this graph.
    //Pure Virtual => Must be overwritten by subclasses!
    virtual bool isAdoptLegal(GraphMLContainer *child)=0;



    //Returns a string graphml representation of this graphml object
    //Pure Virtual => Must be overwritten by subclasses!
    virtual QString toGraphML(qint32 indentationLevel=0)=0;

    //Returns a string representation of this graphml object.
    virtual QString toString()=0;

protected:
    //The list of contained children GraphML elements. (Top level only)
    QVector<GraphMLContainer *> descendants;

    //The list of contained Edge elements in this graph. (Top level only)
    QVector<Edge *> edges;

private:
    //Add an edge which is connected to this graphml object.
    void addEdge(Edge *edge);

    //Remove an edge which is connected to this graphml object.
    void removeEdge(Edge *edge);

    //GraphML Parent
    GraphMLContainer* parent;

    //The unique ID of this graphml object.
    qint32 id;

    //The unique kind ID of this graph object.
    KIND kind;

};

#endif // GRAPHMLCONTAINER_H
