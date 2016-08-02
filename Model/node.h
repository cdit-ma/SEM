#ifndef NODE_H
#define NODE_H

#include "../enumerations.h"
#include "entity.h"
#include "edge.h"
#include <QVariant>

class Node : public Entity
{
    Q_OBJECT

    friend class Edge;
public:
    //Enum for Node Types
    enum NODE_TYPE {NT_NODE, NT_ASPECT, NT_DEFINITION, NT_INSTANCE, NT_IMPL, NT_DEFINSTANCE, NT_HARDWARE, NT_QOS, NT_QOSPROFILE};

    //Constuctor
    Node(NODE_TYPE type = NT_NODE, NODE_CLASS nClass = NC_NONE);
    ~Node();

    QString toGraphML(int indentDepth);
    QString toGraphMLNoVisualData(int indentDepth);
    virtual QString toString();


    virtual VIEW_ASPECT getViewAspect();
    QList<int> getTreeIndex();
    QString getTreeIndexString();

    NODE_CLASS getNodeClass();
    NODE_TYPE getNodeType();
    Node* getContainedAspect();
    int getDepthToAspect();

    void setTop(int index = 0);

    Node* getParentNode(int depth = 1);
    int getParentNodeID();

    //Returns whether or not this Node can Adopt the child Node.
    virtual bool canAdoptChild(Node *node);
    //Adds the Node provided to the list of children.
    void addChild(Node *child);
    QString getNodeKind();

    bool containsChild(Node* child);
    QList<Node *> getChildren(int depth =-1);
    QList<int> getChildrenIDs(int depth =-1);
    QList<Node *> getChildrenOfKind(QString kind, int depth =-1);

    QList<int> getEdgeIDs(Edge::EDGE_CLASS edgeClass = Edge::EC_NONE);
    Node* getFirstChild();
    Edge* getFirstEdge();

    QList<Node *> getSiblings();

    QString toMD5Hash();

    bool isInModel();

    int childrenCount();
    int edgeCount();
    bool hasChildren();
    bool hasEdges();

    //Remove a child Node from this Node
    void removeChild(Node *child);
    //Remove all child Node from this Node

    bool isAncestorOf(GraphML* item);
    bool isDescendantOf(Node *node);

    Edge::EDGE_CLASS canConnect(Node* node);
    virtual bool canConnect_AggregateEdge(Node* aggregate);
    virtual bool canConnect_AssemblyEdge(Node* node);
    virtual bool canConnect_DataEdge(Node* node);
    virtual bool canConnect_DefinitionEdge(Node* definition);
    virtual bool canConnect_DeploymentEdge(Node* hardware);
    virtual bool canConnect_WorkflowEdge(Node* node);
    virtual bool canConnect_QOSEdge(Node* node);

    //Gets the edge that is joining the node to this.
    Edge* getEdgeTo(Node* node);
    bool gotEdgeTo(Node* node);
    Edge* getEdge(int ID);
    bool isIndirectlyConnected(Node* node);
    bool containsEdgeEndPoints(Edge* edge);
    QList<Node *> getAllConnectedNodes();

    QList<Edge *> getEdges(int depth=-1);
    QList<Key *> getKeys(int depth=-1);

    QString getEntityName();
    bool isDefinition();
    bool isInstance();
    bool isAspect();
    bool isImpl();
    bool isHardware();

    void setDefinition(Node *def);
    Node* getDefinition(bool recurse=false);

    void unsetDefinition();

    void addInstance(Node* inst);
    QList<Node*> getInstances();
    void removeInstance(Node* inst);

    void addImplementation(Node* impl);
    QList<Node*> getImplementations();
    QList<Node*> getDependants();
    void removeImplementation(Node* impl);
    bool compareData(Node* node, QString keyName);
    bool compareData(Node* node, QStringList keys);

signals:

    void node_GotDefinition(bool gotDef);
    void node_EdgeAdded(int ID, Edge::EDGE_CLASS edgeClass);
    void node_EdgeRemoved(int ID, Edge::EDGE_CLASS edgeClass);
private:
    void setViewAspect(VIEW_ASPECT aspect);
    void setParentNode(Node* parent, int index);

    QString _toGraphML(int indentDepth, bool ignoreVisuals=false);

    void addEdge(Edge *edge);
    void removeEdge(Edge *edge);



    bool ancestorOf(Node *node);
    bool ancestorOf(Edge* edge);
    bool containsEdge(Edge* edge);

    QList<Node*> getOrderedChildNodes();
    QList<Edge*> getOrderedEdges();

    QList<int> treeIndex;
    QString treeIndexStr;
    int childCount;

    NODE_CLASS nodeClass;

    Node* parentNode;
    Node* definition;

    NODE_TYPE nodeType;

    QList<Node*> instances;
    QList<Node*> implementations;

    QList<Edge::EDGE_CLASS> validEdges;
    //The list of contained children GraphML elements. (Top level only)
    //QMap<int,Node*> orderedChildren;
    QList<Node*> children;

    //The list of contained Edge elements in this graph. (Top level only)
    QList<Edge *> edges;

    VIEW_ASPECT aspect;
protected:
    void setAcceptEdgeClass(Edge::EDGE_CLASS edgeClass);
    bool acceptsEdgeClass(Edge::EDGE_CLASS edgeClass);



};

#endif // NODE_H
