#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "../node.h"
#include <QStringList>

class AggregateInstance : public Node
{
public:
    AggregateInstance();
    ~AggregateInstance();

    // GraphML interface
    QStringList getConnectableKinds();
    
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
private:
    QStringList connectKinds;
};

#endif // AGGREGATEINSTANCE_H
