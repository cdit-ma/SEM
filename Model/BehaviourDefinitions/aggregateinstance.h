#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "../node.h"

class AggregateInstance : public Node
{
public:
    AggregateInstance();
    ~AggregateInstance();

    // GraphML interface
    QString toString();
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);

};

#endif // AGGREGATEINSTANCE_H
