#ifndef EDGE_H
#define EDGE_H

#include "entity.h"

class Node;

//Base class of an edge object (Treated as Class for extended Node Types). Extends the GraphML Abstract Base Class.
class Edge: public Entity{
    Q_OBJECT
public:
    enum EDGE_CLASS{EC_NONE, EC_DEFINITION, EC_AGGREGATE, EC_WORKFLOW, EC_ASSEMBLY, EC_DATA, EC_DEPLOYMENT, EC_QOS, EC_UNDEFINED};
    static QString getKind(EDGE_CLASS edgeClass);

    //Enum for Node Types
    enum EDGE_TYPE {ET_NORMAL, ET_MATCHINGKINDS, ET_AGGREGATE, ET_DEPLOYMENT, ET_ASSEMBLY, ET_COMPONENT, ET_DELEGATE, ET_TERMINATION, ET_DATALINK};

    //Constructor
    Edge(Node* source, Node* destination, EDGE_CLASS edgeClass= EC_NONE);
    ~Edge();

    //Get the source graphml object of this Edge
    Node* getSource();
    int getSourceID();
    int getDestinationID();

    //Get the destination graphml object of this Edge
    Node* getDestination();

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
    EDGE_CLASS getEdgeClass();
private:
    EDGE_TYPE getType();
    EDGE_TYPE type;
    EDGE_CLASS edgeClass;
    Node* source;
    Node* destination;

public:
    bool isInModel();
};

#endif // EDGE_H
