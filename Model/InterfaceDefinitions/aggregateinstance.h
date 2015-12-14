#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "../node.h"
#include "datanode.h"

class AggregateInstance : public DataNode
{
    Q_OBJECT
public:
    AggregateInstance();
    ~AggregateInstance();

    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
    bool canConnect_DataEdge(Node *node);
};

#endif // AGGREGATEINSTANCE_H
