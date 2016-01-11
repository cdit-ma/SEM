#ifndef NODEADAPTER_H
#define NODEADAPTER_H
#include "entityadapter.h"
#include "../Model/node.h"

class NodeAdapter : public EntityAdapter
{
public:
    NodeAdapter(Node* node);

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

    QList<int> getTreeIndex();

    int getParentNodeID(int depth=1);

    NODE_CLASS getNodeClass();
private:
    Node* _node;
    NODE_CLASS _nodeClass;
};

#endif // NODEADAPTER_H
