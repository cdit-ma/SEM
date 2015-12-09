#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class Aggregate;

class EventPort : public Node
{
    Q_OBJECT
public:
    EventPort(bool isInEventPort);
    ~EventPort();

    bool isInEventPort();
    bool isOutEventPort();

    void setAggregate(Aggregate *aggregate);
    void unsetAggregate();
    Aggregate* getAggregate();

    bool canAdoptChild(Node *node);
    bool canConnect_AggregateEdge(Node *aggregate);

private:
    Aggregate* aggregate;
    bool inEventPort;
};

#endif // EVENTPORT_H
