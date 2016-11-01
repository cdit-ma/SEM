#ifndef EDGE_H
#define EDGE_H

#include "entity.h"

class Node;

//Base class of an edge object (Treated as Class for extended Node Types). Extends the GraphML Abstract Base Class.
class Edge: public Entity{
    Q_OBJECT
public:
    enum EDGE_KIND{
        EC_NONE = 0,
        EC_DEFINITION= 1,
        EC_AGGREGATE= 2,
        EC_WORKFLOW= 3,
        EC_ASSEMBLY= 4,
        EC_DATA= 5,
        EC_DEPLOYMENT= 6,
        EC_QOS= 7,
        EC_UNDEFINED = 8};
    static QList<EDGE_KIND> getEdgeKinds();

    static QString getKind(EDGE_KIND edgeClass);
    static EDGE_KIND getEdgeKind(QString kind);

    //Enum for Node Types
    enum EDGE_TYPE {ET_NORMAL, ET_MATCHINGKINDS, ET_AGGREGATE, ET_DEPLOYMENT, ET_ASSEMBLY, ET_COMPONENT, ET_DELEGATE, ET_TERMINATION, ET_DATALINK};

    //Constructor
    Edge(Node* source, Node* destination, EDGE_KIND edgeClass= EC_NONE);
    ~Edge();


    //Get the source graphml object of this Edge
    Node* getSource() const;
    int getSourceID();
    int getDestinationID();

    //Get the destination graphml object of this Edge
    Node* getDestination() const;

    //Return the graphml representation of this
    QString toGraphML(qint32 indentationLevel=0);

    bool isInstanceToInstanceLink();
    bool isInstanceLink();
    bool isImplLink();
    bool isAggregateLink();
    bool isDeploymentLink();
    bool isAssemblyLink();
    bool isComponentLink();
    bool isTerminationLink();
    bool isNormalLink();
    bool isDataLink();

    bool isAssemblyLevelLink();


    bool isDelegateLink();
    bool contains(Node *item);
    QString toString();


    bool isAggregateEdge();
    bool isDefinitionEdge();
    EDGE_KIND getEdgeKind();
private:
    EDGE_TYPE getType();
    EDGE_TYPE type;
    EDGE_KIND edgeClass;
    Node* source;
    Node* destination;

public:
    bool isInModel();
};

#endif // EDGE_H
