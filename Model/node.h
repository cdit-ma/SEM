#ifndef NODE_H
#define NODE_H

#include "graphml.h"
#include "edge.h"
#include <QMap>

#include <QString>

class Node : public GraphML
{
    Q_OBJECT

    //Friend to Edge, meaning Edge can access the private members of node!
    friend class Edge;
public:
    //Enum for Node Types
    enum NODE_TYPE {NT_NODE, NT_ASPECT, NT_DEFINITION, NT_INSTANCE, NT_IMPL, NT_DEFINSTANCE};

    //Constuctor
    Node(NODE_TYPE type = NT_NODE);

    QList<int> getTreeIndex();
    QString getTreeIndexString();
    ~Node();

signals:
    //Used to construct and tear down GUI elements.
    void constructGUI(Node*);
    void destructGUI(Node*, QString ID);


public:
    virtual QStringList getConnectableKinds();
    QList<Node*> getItemsConnectedLeft();
    QList<Node*> getItemsConnectedRight();

    int getIndirectConnectCount(QString nodeKind);
    //Child Node Methods
    void setTop();
    virtual QString toString();
    Node* getParentNode();

    //Returns whether or not this Node can Adopt the child Node.
    virtual bool canAdoptChild(Node *node);

    //Adds the Node provided to the list of children.
    void addChild(Node *child);

    QString getNodeKind();

    bool containsChild(Node* child);

     //Gets the Children of this Node to a desired Depth.
    //-1 = Recurse forever.
    QList<Node *> getChildren(int depth =-1);

    QList<Node *> getSiblings();

    QList<Node*> getNodesLeft();
    QList<Node*> getNodesRight();

    QList<Node *> getChildrenOfKind(QString kindStr, int depth =-1);

    //Node* getChild(int position);


    int childrenCount();
    int edgeCount();

    //Remove a child Node from this Node
    void removeChild(Node *child);

    //Remove all child Node from this Node
    void removeChildren();

    //Checks if the Node is a (distant) Parent.
    bool ancestorOf(Node *node);

    bool ancestorOf(Edge* edge);
    bool isAncestorOf(GraphML* item);
    //Checks if the Node is a (distant) child.
    bool isDescendantOf(Node *node);


    //Edge Methods

    //Returns whether or not this Node can connect with the provided Node
    virtual bool canConnect(Node* node);

    //Gets the edge that is joining the node to this.
    Edge* getConnectingEdge(Node *node);

    QList<Node*> getAllConnectedNodes(QList<Node*> connectedNode = QList<Node*>());

    //Returns true if this Node is directly connected to the provided node.
    bool isConnected(Node* node);

    bool isIndirectlyConnected(Node* node);




    bool containsEdge(Edge* edge);

    //Gets list of edges which are connected to this Node.
    QList<Edge *> getEdges(int depth=-1 );

    QList<GraphMLKey *> getKeys(int depth=-1 );

    //Removes all edges.
    void removeEdges();



    //Visual Methods

    QString toGraphML(qint32 indentationLevel=0);


    //Specific DIG Methods.
    bool isDefinition();
    bool isInstance();
    bool isAspect();
    bool isImpl();

    void setDefinition(Node *def);
    Node* getDefinition();
    void unsetDefinition();

    void addInstance(Node* inst);
    QList<Node*> getInstances();
    void removeInstance(Node* inst);

    void addImplementation(Node* impl);
    QList<Node*> getImplementations();
    void removeImplementation(Node* impl);




private:
    //Remove an edge which is connected to this graphml object.
    void addEdge(Edge *edge);
    void removeEdge(Edge *edge);
    void setParentNode(Node* parent, int index);



    QList<Node*> getOrderedChildNodes();
    QList<Edge*> getOrderedEdges();

    QList<int> treeIndex;
    QString treeIndexStr;
    int childCount;

    Node* parentNode;
    Node* definition;

    NODE_TYPE nodeType;

    QList<Node*> instances;
    QList<Node*> implementations;

    //The list of contained children GraphML elements. (Top level only)
    //QMap<int,Node*> orderedChildren;
    QList<Node*> children;

    //The list of contained Edge elements in this graph. (Top level only)
    QList<Edge *> edges;

};

#endif // NODE_H
