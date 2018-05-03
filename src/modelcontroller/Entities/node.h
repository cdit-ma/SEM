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

    public:
        enum class EdgeRule{
            ALLOW_EXTERNAL_DEFINITIONS,
            IGNORE_REQUIRED_INSTANCE_DEFINITIONS,
            ALWAYS_CHECK_VALID_DEFINITIONS
        };

    

    
    

    protected:
        static void RegisterNodeKind(EntityFactory* factory, NODE_KIND kind, QString kind_string, std::function<Node* ()> constructor);
        static void RegisterDefaultData(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, bool is_protected = false, QVariant value = QVariant());
        static void RegisterValidDataValues(EntityFactory* factory, NODE_KIND kind, QString key_name, QVariant::Type type, QList<QVariant> values);

        static void BindDefinitionToInstance(Node* definition, Node* instance, bool setup);
    public:
        static void BindDataRelationship(Node* source, Node* destination, bool setup);
    signals:
        void acceptedEdgeKindsChanged(Node* node);
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

        void addInstanceKind(NODE_KIND kind);
        void addImplsDefinitionKind(NODE_KIND kind);
        void addInstancesDefinitionKind(NODE_KIND kind);
        
        void addImplKind(NODE_KIND kind);
        void setChainableDefinition();
        
    public:
        bool isValidInstanceKind(NODE_KIND kind) const;
        bool isValidImplKind(NODE_KIND kind) const;
        bool isValidDefinitionKind(NODE_KIND kind) const;

    
        QSet<NODE_KIND> getInstanceKinds() const;
        QSet<NODE_KIND> getDefinitionKinds() const;
        QSet<NODE_KIND> getImplKinds() const;

        
        virtual QSet<Node*> getDependants() const;

        QList<Node*> getRequiredInstanceDefinitions();
        virtual QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
        virtual QSet<Node*> getParentNodesForValidDefinition();


        QString toGraphML(int indentDepth = 0, bool functional_export = false);

    protected:
        virtual void childAdded(Node* child){};
        virtual void childRemoved(Node* child);
        virtual void parentSet(Node* parent){};
        virtual void definitionSet(Node* definition){};

    protected:
        void SetEdgeRuleActive(EdgeRule rule, bool active = true);
        bool IsEdgeRuleActive(EdgeRule rule);
    private:

        QSet<EdgeRule> active_edge_rules;
    public:

        virtual VIEW_ASPECT getViewAspect() const;
    

    int getDepth() const;
    QList<int> getTreeIndex();
    QString getTreeIndexAlpha();

    

    NODE_KIND getNodeKind() const;
    NODE_KIND getParentNodeKind() const;

    bool isAttached() const;

    bool canConstructChildren() const;


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
    QList<Node *> getChildrenOfKinds(QSet<NODE_KIND> kinds, int depth =-1);


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
    NODE_KIND parent_node_kind = NODE_KIND::NONE;
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
public:
    QSet<NODE_KIND> getAcceptedNodeKinds() const;
protected:
    void setAcceptsNodeKind(NODE_KIND node_kind, bool accept = true);
    bool canAcceptNodeKind(NODE_KIND node_kind) const;
    bool canAcceptNodeKind(const Node* node) const;
public:
    virtual QSet<NODE_KIND> getUserConstructableNodeKinds() const;
private:
    QSet<NODE_KIND> accepted_node_kinds_;
    
    QSet<EDGE_KIND> accepted_edge_kinds_as_source_;
    QSet<EDGE_KIND> accepted_edge_kinds_as_target_;


    QSet<NODE_KIND> definition_kinds_;
    QSet<NODE_KIND> instance_kinds_;
    QSet<NODE_KIND> impl_kinds_;
protected:
    void setTop(int index = 0);
    void setNodeType(NODE_TYPE type);
    void removeNodeType(NODE_TYPE type);

    
    
    void setAcceptsEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction, bool accept = true);
    bool canAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const;


public:
    QSet<EDGE_KIND> getCurrentAcceptedEdgeKind(EDGE_DIRECTION direction) const;
    QSet<EDGE_KIND> getAcceptedEdgeKind(EDGE_DIRECTION direction) const;

public:
    bool canCurrentlyAcceptEdgeKind(EDGE_KIND edge_kind, EDGE_DIRECTION direction) const;
    bool canCurrentlyAcceptEdgeKind(EDGE_KIND edgeKind, Node* dst) const;
public:
    bool isNodeOfType(NODE_TYPE type) const;


    
    
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node* dst);
};

inline uint qHash(Node::EdgeRule key, uint seed){
    return ::qHash(static_cast<uint>(key), seed);
};

#endif // NODE_H
