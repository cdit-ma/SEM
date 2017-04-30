#ifndef NODE_H
#define NODE_H

#include "../enumerations.h"
#include "entity.h"
#include "edge.h"
#include <QVariant>
#include <QSet>

#include "nodekinds.h"

enum class NODE_TYPE;
enum class NODE_KIND;

class Node : public Entity
{
    Q_OBJECT

    friend class Edge;
    friend class EntityFactory;
public:
    
    

    protected:
        //Constuctor
        Node(NODE_KIND kind);

        void addValidValue(QString key_name, QVariant value);
        void addValidValues(QString key_name, QList<QVariant> values);
        QStringList getValidValueKeys();
        QList<QVariant> getValidValues(QString key_name);
    public:
        ~Node();

    QString toGraphML(int indentDepth);
    QString toGraphMLNoVisualData(int indentDepth);

    virtual VIEW_ASPECT getViewAspect() const;
    

    QList<int> getTreeIndex();
    QString getTreeIndexAlpha();

    NODE_KIND getNodeKind() const;

    bool isAttached() const;


    int getDepthFromAspect();
    int getDepthFromCommonAncestor(Node* dst);

    Node* getCommonAncestor(Node* dst);



    Node* getParentNode(int depth = 1);
    int getParentNodeID();

    //Returns whether or not this Node can Adopt the child Node.
    virtual bool canAdoptChild(Node *node);
    //Adds the Node provided to the list of children.
    bool addChild(Node *child);
    bool setAsRoot(int root_index);
    QString getNodeKindStr();

    bool indirectlyConnectedTo(Node* node);

    bool containsChild(Node* child);
    QList<Node *> getChildren(int depth =-1) const;
    QList<int> getChildrenIDs(int depth =-1);
    QList<Node *> getChildrenOfKind(QString kindStr, int depth =-1);
    QList<Node *> getChildrenOfKind(NODE_KIND kind, int depth =-1);


    QList<int> getEdgeIDs(Edge::EDGE_KIND edgeClass = Edge::EC_NONE);
    Node* getFirstChild();

    QList<Node *> getSiblings();

    QString toMD5Hash();

    bool isInModel();

    int childrenCount();
    bool hasChildren();
    bool hasEdges();

    //Remove a child Node from this Node
    bool removeChild(Node *child);

    bool isAncestorOf(GraphML* item);
    bool isDescendantOf(Node *node);

    //Gets the edge that is joining the node to this.
    Edge* getEdgeTo(Node* node, Edge::EDGE_KIND edgeKind = Edge::EC_NONE);
    bool gotEdgeTo(Node* node, Edge::EDGE_KIND edgeKind = Edge::EC_NONE);

    QList<Edge *> getEdges(int depth=-1, Edge::EDGE_KIND edgeKind = Edge::EC_NONE) const;
    QList<Key *> getKeys(int depth=-1);

    bool isDefinition() const;
    bool isInstance() const;
    bool isInstanceImpl() const;
    bool isAspect() const;
    bool isImpl() const;

    QSet<NODE_TYPE> getTypes() const;

    void setDefinition(Node *def);
    Node* getDefinition(bool recurse=false) const;
    void unsetDefinition();

    void addInstance(Node* inst);
    QList<Node*> getInstances() const;
    void removeInstance(Node* inst);

    void addImplementation(Node* impl);
    QList<Node*> getImplementations() const;
    QList<Node*> getNestedDependants() const;
    QList<Node*> getDependants() const;

    void removeImplementation(Node* impl);

    bool compareData(Node* node, QString keyName);
    bool compareData(Node* node, QStringList keys);

signals:
    void childCountChanged();
private:
    void setViewAspect(VIEW_ASPECT aspect);
    void setParentNode(Node* parent, int index);

    QString _toGraphML(int indentDepth, bool ignoreVisuals=false);

    void addEdge(Edge *edge);
    void removeEdge(Edge *edge);



    bool ancestorOf(Node *node);
    bool ancestorOf(Edge* edge);
    bool containsEdge(Edge* edge);

    QList<Node*> getOrderedChildNodes() const;

    QList<int> treeIndex;
    QString treeIndexString;
    int childCount;

    Node* parentNode;
    Node* definition;

    NODE_KIND nodeKind;
    NODE_TYPE nodeType;

    QList<Node*> instances;
    QList<Node*> implementations;




    //The list of contained children GraphML elements. (Top level only)
    QList<Node*> children;

    //The list of contained Edge elements in this graph. (Top level only)

    QMultiMap<Edge::EDGE_KIND, Edge*> edges;

    //List of valid values for keys 
    QMultiMap<QString, QVariant> valid_values_;


    VIEW_ASPECT aspect;


    QSet<NODE_TYPE> types;
    QList<Edge::EDGE_KIND> validEdgeKinds;

protected:
    void setTop(int index = 0);
    void setNodeType(NODE_TYPE type);
    void removeNodeType(NODE_TYPE type);
    void setAcceptsEdgeKind(Edge::EDGE_KIND edgeKind);
    void removeEdgeKind(Edge::EDGE_KIND edgeKind);
public:
    bool isNodeOfType(NODE_TYPE type) const;
    bool acceptsEdgeKind(Edge::EDGE_KIND edgeKind) const;

    QList<Edge::EDGE_KIND> getAcceptedEdgeKinds() const;
    virtual bool requiresEdgeKind(Edge::EDGE_KIND edgeKind) const;
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node* dst);
};

#endif // NODE_H
