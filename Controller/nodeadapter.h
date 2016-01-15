#ifndef NODEADAPTER_H
#define NODEADAPTER_H
#include "entityadapter.h"
#include "../Model/node.h"

class NodeAdapter : public EntityAdapter
{
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

    int childCount();
    int edgeCount();

    bool isBehaviourAdapter();



    QList<int> getTreeIndex();

    int getParentNodeID(int depth=1);

    NODE_CLASS getNodeClass();
private:
    Node* _node;
    NODE_CLASS _nodeClass;
    NODE_ADAPTER_KIND _nodeAdapterKind;
protected:
    Node* getNode();
};

#endif // NODEADAPTER_H
