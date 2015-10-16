#ifndef EDGE_H
#define EDGE_H

#include "graphml.h"
#include <QString>

class Node;

//Base class of an edge object (Treated as Class for extended Node Types). Extends the GraphML Abstract Base Class.
class Edge: public GraphML{
    Q_OBJECT
public:
    //Enum for Node Types
    enum EDGE_TYPE {ET_NORMAL, ET_MATCHINGKINDS, ET_AGGREGATE, ET_DEPLOYMENT, ET_ASSEMBLY, ET_COMPONENT, ET_DELEGATE, ET_TERMINATION};

    //Constructor
    Edge(Node* source, Node* destination);
    ~Edge();

    //Get the source graphml object of this Edge
    Node* getSource();

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

    bool isAssemblyLevelLink();


    bool isDelegateLink();
    bool contains(Node *item);
    QString toString();


private:
    EDGE_TYPE getType();
    EDGE_TYPE type;
    Node* source;
    Node* destination;

};

#endif // EDGE_H
