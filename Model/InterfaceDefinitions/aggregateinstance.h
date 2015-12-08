#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "../node.h"

class AggregateInstance : public Node
{
    Q_OBJECT
public:
    AggregateInstance();
    ~AggregateInstance();

    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
};

#endif // AGGREGATEINSTANCE_H
