#ifndef NODEADAPTER_H
#define NODEADAPTER_H
#include <QObject>
#include "entityadapter.h"
#include "../Model/node.h"
#include "../Model/edge.h"

class NodeAdapter : public EntityAdapter
{
    Q_OBJECT
public:
    enum NODE_ADAPTER_KIND {NAK_NODE, NAK_BEHAVIOUR};
    NodeAdapter(Node* node, NODE_ADAPTER_KIND nodeAdapterKind=NAK_NODE);

    bool isDefinition();
    bool isInstance();
    bool isAspect();
    bool isImpl();
    bool isHardware();
    bool gotDefinition();
    int getDefinitionID();
    bool gotInstances();
    bool gotImpl();

    VIEW_ASPECT getViewAspect();

    uint getSelectionID();


    int childCount();
    int edgeCount();

    bool isBehaviourAdapter();

    QList<int> getEdgeIDs(Edge::EDGE_CLASS edgeClass = Edge::EC_NONE);

    int getChildEdgeSrcID(int edgeID);
    int getChildEdgeDstID(int edgeID);


    QList<int> getTreeIndex();

    int getParentNodeID(int depth=1);

    NODE_CLASS getNodeClass();

signals:
    void edgeAdded(int ID, Edge::EDGE_CLASS edgeClass);
    void edgeRemoved(int ID, Edge::EDGE_CLASS edgeClass);
    void gotDefinition(bool gotDefinition);

private:
    Node* _node;
    NODE_CLASS _nodeClass;
    NODE_ADAPTER_KIND _nodeAdapterKind;
protected:
    Node* getNode();
};

#endif // NODEADAPTER_H
