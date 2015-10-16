#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "../node.h"
#include <QStringList>

class AggregateInstance : public Node
{
public:
    AggregateInstance();
    ~AggregateInstance();

    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // AGGREGATEINSTANCE_H
