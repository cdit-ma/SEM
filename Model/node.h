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
    enum NODE_KIND{NK_NONE,
                   NK_MODEL,
                   NK_WORKER_DEFINITIONS,

                   NK_INTERFACE_DEFINITIONS,
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

                   NK_DEPLOYMENT_DEFINITIONS,
                   NK_ASSEMBLY_DEFINITIONS,
                   NK_HARDWARE_DEFINITIONS,
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


                   NK_BEHAVIOUR_DEFINITIONS,
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
                   NK_QOS_DDS_POLICY_WRITERDATALIFECYCLE,
                  };
    //Enum for Node Types
    enum NODE_TYPE {NT_NODE, NT_ASPECT, NT_DEFINITION, NT_INSTANCE, NT_IMPL, NT_HARDWARE, NT_QOS, NT_QOS_PROFILE, NT_BEHAVIOUR, NT_DATA, NT_EVENTPORT, NT_EVENTPORT_ASSEMBLY, NT_QOS_DDS_POLICY, NT_BRANCH, NT_PARAMETER};
    //Constuctor
    Node(NODE_KIND kind = NK_NONE);
    Node(NODE_TYPE type = NT_NODE, NODE_CLASS nClass = NC_NONE);
    ~Node();

    QString toGraphML(int indentDepth);
    QString toGraphMLNoVisualData(int indentDepth);
    virtual QString toString();


    virtual VIEW_ASPECT getViewAspect();
    QList<int> getTreeIndex();
    QString getTreeIndexString();
    QString getTreeIndexAlpha();

    NODE_KIND getNodeKind() const;
    NODE_CLASS getNodeClass();
    NODE_TYPE getNodeType();
    Node* getContainedAspect();
    int getDepthToAspect();

    Node* getCommonAncestor(Node* dst);
    int getHeightToCommonAncestor(Node* dst);

    void setTop(int index = 0);

    Node* getParentNode(int depth = 1);
    int getParentNodeID();

    //Returns whether or not this Node can Adopt the child Node.
    virtual bool canAdoptChild(Node *node);
    //Adds the Node provided to the list of children.
    void addChild(Node *child);
    QString getNodeKindStr();

    bool containsChild(Node* child);
    QList<Node *> getChildren(int depth =-1);
    QList<int> getChildrenIDs(int depth =-1);
    QList<Node *> getChildrenOfKind(QString kindStr, int depth =-1);
    QList<Node *> getChildrenOfKind(Node::NODE_KIND kind, int depth =-1);


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


    virtual bool canConnect(Node* node, Edge::EDGE_CLASS edgeClass);




    Edge::EDGE_CLASS canConnect(Node* node);
    virtual bool canConnect_AggregateEdge(Node* aggregate);
    virtual bool canConnect_AssemblyEdge(Node* node);
    virtual bool canConnect_DataEdge(Node* node);
    virtual bool canConnect_DefinitionEdge(Node* definition);


    virtual bool canConnect_DeploymentEdge(Node* hardware);
    virtual bool canConnect_WorkflowEdge(Node* node);
    virtual bool canConnect_QOSEdge(Node* node);

    //Gets the edge that is joining the node to this.
    Edge* getEdgeTo(Node* node, Edge::EDGE_CLASS edgeKind = Edge::EC_NONE);
    bool gotEdgeTo(Node* node);
    Edge* getEdge(int ID);
    bool isIndirectlyConnected(Node* node);
    bool containsEdgeEndPoints(Edge* edge);
    QList<Node *> getAllConnectedNodes();

    QList<Edge *> getEdges(int depth=-1, Edge::EDGE_CLASS edgeKind = Edge::EC_NONE);
    QList<Key *> getKeys(int depth=-1);

    QString getEntityName();
    bool isDefinition();
    bool isInstance();
    bool isInstanceImpl();
    bool isAspect();
    bool isImpl();

    bool canAcceptEdgeClass(Edge::EDGE_CLASS edgeClass);

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
    QList<Edge*> getOrderedEdges(Edge::EDGE_CLASS edgeKind = Edge::EC_NONE);

    QList<int> treeIndex;
    QString treeIndexStr;
    QString treeIndexStr2;
    int childCount;

    NODE_CLASS nodeClass;

    Node* parentNode;
    Node* definition;

    NODE_KIND nodeKind;
    NODE_TYPE nodeType;

    QList<Node*> instances;
    QList<Node*> implementations;


    QList<Edge::EDGE_CLASS> validEdges2;

    QList<Edge::EDGE_CLASS> validEdges;



    //The list of contained children GraphML elements. (Top level only)
    //QMap<int,Node*> orderedChildren;
    QList<Node*> children;

    //The list of contained Edge elements in this graph. (Top level only)
    QList<Edge *> edges;


    VIEW_ASPECT aspect;

    QList<NODE_TYPE> types;
    QList<Edge::EDGE_CLASS> validEdgeKinds;

    QList<Edge::EDGE_CLASS> acceptableEdges;
    QList<Edge::EDGE_CLASS> requiredEdges;

public:

    virtual bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node* dst);




protected:
    void setNodeType(NODE_TYPE type);
    void setAcceptsEdgeKind(Edge::EDGE_CLASS edgeKind);
public:
    //static bool isNodeOfType(Node* node, NODE_TYPE type);
    bool isNodeOfType(NODE_TYPE type) const;
    bool acceptsEdgeKind(Edge::EDGE_CLASS edgeKind) const;

};

#endif // NODE_H
