#ifndef NODE_H
#define NODE_H

#include "graphml.h"

#include <QString>

class Node : public GraphML
{
    Q_OBJECT

    //Friend to Edge, meaning Edge can access the private members of node!
    friend class Edge;
public:
    //Enum for Node Types
    enum NODE_TYPE {NT_NODE, NT_DEFINITION, NT_INSTANCE, NT_IMPL};

    //Constuctor
    Node(NODE_TYPE type = NT_NODE);
    ~Node();

signals:
    //Used to construct and tear down GUI elements.
    void constructGUI(Node*);
    void destructGUI(Node*, QString ID);


public:
    //Child Node Methods

    Node* getParentNode();

    //Returns whether or not this Node can Adopt the child Node.
    virtual bool canAdoptChild(Node *node);

    //Adds the Node provided to the list of children.
    void addChild(Node *child);

    bool containsChild(Node* child);

    //Gets the Children of this Node to a desired Depth.
    //-1 = Recurse forever.
    QVector<Node *> getChildren(int depth =-1);

    int childrenCount();

    //Remove a child Node from this Node
    void removeChild(Node *child);

    //Remove all child Node from this Node
    void removeChildren();

    //Checks if the Node is a (distant) Parent.
    bool isAncestorOf(Node *node);

    //Checks if the Node is a (distant) child.
    bool isDescendantOf(Node *node);


    //Edge Methods

    //Returns whether or not this Node can connect with the provided Node
    virtual bool canConnect(Node* node);

    //Gets the edge that is joining the node to this.
    Edge* getConnectingEdge(Node *node);

    //Returns true if this Node is connected to the provided node.
    bool isConnected(Node* node);


    bool containsEdge(Edge* edge);

    //Gets list of edges which are connected to this Node.
    QVector<Edge *> getEdges(int depth=-1 );
    QVector<GraphMLKey *> getKeys(int depth=-1 );

    //Removes all edges.
    void removeEdges();


    //Searching Methods

    //Visual Methods
    void addAspect(QString aspect);
    void removeAspect(QString aspect);
    bool isInAspect(QString aspect);

    QString toGraphML(qint32 indentationLevel=0);


    //Specific DIG Methods.
    bool isDefinition();
    bool isInstance();
    bool isImpl();

    void setDefinition(Node *def);
    Node* getDefinition();
    void unsetDefinition();

    void addInstance(Node* inst);
    QVector<Node*> getInstances();
    void removeInstance(Node* inst);

    void setImplementation(Node* impl);
    Node* getImplementation();
    void unsetImplementation();


private:
    //Remove an edge which is connected to this graphml object.
    void addEdge(Edge *edge);
    void removeEdge(Edge *edge);
    void setParentNode(Node* parent);




    NODE_TYPE nodeType;

    //Used Sparingly
    QVector<Node *> instances;

    Node* definition;
    Node* implementation;

    Node* parentNode;

    //Graph* childGraph;
    QVector<QString> containedAspects;

    //The list of contained children GraphML elements. (Top level only)
    QVector<Node *> children;

    //The list of contained Edge elements in this graph. (Top level only)
    QVector<Edge *> edges;

};

#endif // NODE_H
