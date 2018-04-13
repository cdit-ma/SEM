#ifndef NODE_H
#define NODE_H

#include "entity.h"

#include "../nodekinds.h"
#include "../edgekinds.h"

#include <QVariant>
#include <QSet>
#include <functional>
enum class VIEW_ASPECT;

class Edge;
class EntityFactory;
class Node : public Entity
{
    Q_OBJECT
    friend class Edge;
    friend class EntityFactory;

    protected:
        static void RegisterNodeKind(EntityFactory* factory, NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor);
        static void RegisterDefaultData(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
        static void RegisterValidDataValues(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values);

        static void BindDefinitionToInstance(Node* definition, Node* instance, bool setup);
    public:
        static void BindDataRelationship(Node* source, Node* destination, bool setup);
    protected:

        //Constuctor
        Node(NODE_KIND kind);
        ~Node();

        //Entity Factory Constructor
        Node(EntityFactory* factory, NODE_KIND kind, QString kind_string);


        void addValidValue(QString key_name, QVariant value);
        void addValidValues(QString key_name, QList<QVariant> values);
        QStringList getValidValueKeys();
        QList<QVariant> getValidValues(QString key_name);

        void setInstanceKind(NODE_KIND kind);
        void setDefinitionKind(NODE_KIND kind);
        void setImplKind(NODE_KIND kind);
    public:
        NODE_KIND getInstanceKind() const;
        NODE_KIND getDefinitionKind() const;
        NODE_KIND getImplKind() const;

        QString toGraphML(int indentDepth = 0, bool functional_export = false);

    protected:
        virtual void childAdded(Node* child){};
        virtual void childRemoved(Node* child);
        virtual void parentSet(Node* parent){};
    public:

        virtual VIEW_ASPECT getViewAspect() const;
    

    int getDepth() const;
    QList<int> getTreeIndex();
    QString getTreeIndexAlpha();

    NODE_KIND getNodeKind() const;

    bool isAttached() const;


    int getDepthFromAspect();
    int getDepthFromCommonAncestor(Node* dst);

    Node* getCommonAncestor(Node* dst);


    QList<Node*> getParentNodes(int depth = 1);
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
    QList<Node *> getChildren(int depth =-1);
    QList<int> getChildrenIDs(int depth =-1);
    QList<Node *> getChildrenOfKind(QString kindStr, int depth =-1);
    QList<Node *> getChildrenOfKind(NODE_KIND kind, int depth =-1);


    QList<int> getEdgeIDs(EDGE_KIND edgeClass);
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
    Edge* getEdgeTo(Node* node, EDGE_KIND edgeKind);
    bool gotEdgeTo(Node* node, EDGE_KIND edgeKind);

    QList<Edge*> getAllEdges();

    bool gotEdgeKind(EDGE_KIND kind);
    QList<Edge *> getEdges(int depth, EDGE_KIND edgeKind);
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
    QSet<Node*> getNestedDependants();
    QSet<Node*> getDependants() const;

    void removeImplementation(Node* impl);

    bool compareData(Node* node, QString keyName);
    bool compareData(Node* node, QStringList keys);

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

    QList<int> treeIndex;
    QString treeIndexString;
    int childCount;

    Node* parentNode = 0;
    Node* definition = 0;;

    NODE_KIND nodeKind;
    NODE_TYPE nodeType;

    QList<Node*> instances;
    QList<Node*> implementations;




    //The list of contained children GraphML elements. (Top level only)
    QList<Node*> children;

    //The list of contained Edge elements in this graph. (Top level only)

    QMultiMap<EDGE_KIND, Edge*> edges;

    //List of valid values for keys 
    QMultiMap<QString, QVariant> valid_values_;


    VIEW_ASPECT aspect;


    QSet<NODE_TYPE> types;
    QSet<EDGE_KIND> valid_edge_kinds;

    NODE_KIND definition_kind_;
    NODE_KIND instance_kind_;
    NODE_KIND impl_kind_;

    bool rule_definitionedge_ignore_parent_definition = false;;
protected:
    void setTop(int index = 0);
    void setNodeType(NODE_TYPE type);
    void removeNodeType(NODE_TYPE type);
    void setAcceptsEdgeKind(EDGE_KIND edgeKind, bool accept = true);
    void removeEdgeKind(EDGE_KIND edgeKind);

public:
    bool isNodeOfType(NODE_TYPE type) const;
    bool acceptsEdgeKind(EDGE_KIND edgeKind) const;
    bool requiresEdgeKind(EDGE_KIND edge_kind) const;

    QSet<EDGE_KIND> getValidEdgeKinds() const;
    QSet<EDGE_KIND> getRequiredEdgeKinds() const;
    
    
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node* dst);
};

#endif // NODE_H
