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
    enum NODE_KIND
    {
        NK_NONE,
        NK_MODEL,
        NK_INTERFACE_DEFINITIONS,
        NK_BEHAVIOUR_DEFINITIONS,
        NK_DEPLOYMENT_DEFINITIONS,
        NK_ASSEMBLY_DEFINITIONS,
        NK_HARDWARE_DEFINITIONS,
        NK_WORKER_DEFINITIONS,

        NK_AGGREGATE,
        NK_AGGREGATE_INSTANCE,
        NK_MEMBER,
        NK_VECTOR_INSTANCE,
        NK_COMPONENT,
        NK_MEMBER_INSTANCE,
        NK_WORKLOAD_DEFINITIONS,
        NK_ATTRIBUTE,
        NK_BLACKBOX,
        NK_INEVENTPORT,
        NK_OUTEVENTPORT,
        NK_IDL,
        NK_VECTOR,

        NK_COMPONENT_ASSEMBLY,
        NK_MANAGEMENT_COMPONENT,
        NK_ATTRIBUTE_INSTANCE,
        NK_BLACKBOX_INSTANCE,
        NK_COMPONENT_INSTANCE,
        NK_INEVENTPORT_DELEGATE,
        NK_OUTEVENTPORT_DELEGATE,
        NK_INEVENTPORT_INSTANCE,
        NK_OUTEVENTPORT_INSTANCE,
        NK_HARDWARE_CLUSTER,
        NK_HARDWARE_NODE,

        NK_ATTRIBUTE_IMPL,
        NK_BRANCH,
        NK_BRANCH_STATE,
        NK_COMPONENT_IMPL,
        NK_INEVENTPORT_IMPL,
        NK_OUTEVENTPORT_IMPL,
        NK_INPUTPARAMETER,
        NK_RETURNPARAMETER,
        NK_PERIODICEVENT,
        NK_PROCESS,
        NK_WORKER_PROCESS,
        NK_TERMINATION,
        NK_VARIABLE,
        NK_WHILELOOP,
        NK_WORKLOAD,
        NK_CONDITION,

        NK_QOS_DDS_PROFILE,
        NK_QOS_DDS_POLICY_DEADLINE,
        NK_QOS_DDS_POLICY_DESTINATIONORDER,
        NK_QOS_DDS_POLICY_DURABILITY,
        NK_QOS_DDS_POLICY_DURABILITYSERVICE,
        NK_QOS_DDS_POLICY_ENTITYFACTORY,
        NK_QOS_DDS_POLICY_GROUPDATA,
        NK_QOS_DDS_POLICY_HISTORY,
        NK_QOS_DDS_POLICY_LATENCYBUDGET,
        NK_QOS_DDS_POLICY_LIFESPAN,
        NK_QOS_DDS_POLICY_LIVELINESS,
        NK_QOS_DDS_POLICY_OWNERSHIP,
        NK_QOS_DDS_POLICY_OWNERSHIPSTRENGTH,
        NK_QOS_DDS_POLICY_PARTITION,
        NK_QOS_DDS_POLICY_PRESENTATION,
        NK_QOS_DDS_POLICY_READERDATALIFECYCLE,
        NK_QOS_DDS_POLICY_RELIABILITY,
        NK_QOS_DDS_POLICY_RESOURCELIMITS,
        NK_QOS_DDS_POLICY_TIMEBASEDFILTER,
        NK_QOS_DDS_POLICY_TOPICDATA,
        NK_QOS_DDS_POLICY_TRANSPORTPRIORITY,
        NK_QOS_DDS_POLICY_USERDATA,
        NK_QOS_DDS_POLICY_WRITERDATALIFECYCLE
    };
    //Enum for Node Types
    enum NODE_TYPE{
        NT_DEFINITION           = 1 << 0,   //bin. 0000 0000 0000 0001
        NT_INSTANCE             = 1 << 1,   //bin. 0000 0000 0000 0010
        NT_IMPLEMENTATION       = 1 << 2,   //bin. 0000 0000 0000 0100
        NT_ASPECT               = 1 << 3,   //bin. 0000 0000 0000 1000
        NT_BEHAVIOUR            = 1 << 4,   //bin. 0000 0000 0001 0000
        NT_DATA                 = 1 << 5,   //bin. 0000 0000 0010 0000
        NT_EVENTPORT            = 1 << 6,   //bin. 0000 0000 0100 0000
        NT_EVENTPORT_ASSEMBLY   = 1 << 7,   //bin. 0000 0000 1000 0000
        NT_HARDWARE             = 1 << 8,   //bin. 0000 0001 0000 0000
        NT_QOS                  = 1 << 9,   //bin. 0000 0010 0000 0000
        NT_QOS_PROFILE          = 1 << 10,  //bin. 0000 0100 0000 0000
        NT_DDS                  = 1 << 11,  //bin. 0000 1000 0000 0000
        NT_BRANCH               = 1 << 12,  //bin. 0001 0000 0000 0000
        NT_PARAMETER            = 1 << 13,  //bin. 0010 0000 0000 0000
    };

    //Constuctor
    Node(NODE_KIND kind = NK_NONE);
    ~Node();

    QString toGraphML(int indentDepth);
    QString toGraphMLNoVisualData(int indentDepth);

    virtual VIEW_ASPECT getViewAspect() const;
    QList<int> getTreeIndex();
    QString getTreeIndexAlpha();

    NODE_KIND getNodeKind() const;


    int getDepthFromAspect();
    int getDepthFromCommonAncestor(Node* dst);

    Node* getCommonAncestor(Node* dst);



    Node* getParentNode(int depth = 1);
    int getParentNodeID();

    //Returns whether or not this Node can Adopt the child Node.
    virtual bool canAdoptChild(Node *node);
    //Adds the Node provided to the list of children.
    void addChild(Node *child);
    QString getNodeKindStr();

    bool indirectlyConnectedTo(Node* node);

    bool containsChild(Node* child);
    QList<Node *> getChildren(int depth =-1);
    QList<int> getChildrenIDs(int depth =-1);
    QList<Node *> getChildrenOfKind(QString kindStr, int depth =-1);
    QList<Node *> getChildrenOfKind(Node::NODE_KIND kind, int depth =-1);


    QList<int> getEdgeIDs(Edge::EDGE_KIND edgeClass = Edge::EC_NONE);
    Node* getFirstChild();
    Edge* getFirstEdge();

    QList<Node *> getSiblings();

    QString toMD5Hash();

    bool isInModel();

    int childrenCount();
    bool hasChildren();
    bool hasEdges();

    //Remove a child Node from this Node
    void removeChild(Node *child);

    bool isAncestorOf(GraphML* item);
    bool isDescendantOf(Node *node);

    //Gets the edge that is joining the node to this.
    Edge* getEdgeTo(Node* node, Edge::EDGE_KIND edgeKind = Edge::EC_NONE);
    bool gotEdgeTo(Node* node, Edge::EDGE_KIND edgeKind = Edge::EC_NONE);

    QList<Edge *> getEdges(int depth=-1, Edge::EDGE_KIND edgeKind = Edge::EC_NONE);
    QList<Key *> getKeys(int depth=-1);

    bool isDefinition() const;
    bool isInstance() const;
    bool isInstanceImpl() const;
    bool isAspect() const;
    bool isImpl() const;

    int getTypes() const;

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
    QList<Edge*> getOrderedEdges(Edge::EDGE_KIND edgeKind = Edge::EC_NONE);

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


    VIEW_ASPECT aspect;

    int types;
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
    bool requiresEdgeKind(Edge::EDGE_KIND edgeKind) const;
    QList<Edge::EDGE_KIND> getAcceptedEdgeKinds() const;
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node* dst);
};

#endif // NODE_H
