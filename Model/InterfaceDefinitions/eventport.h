#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class Aggregate;

class EventPort : public Node
{
public:
    EventPort();
    ~EventPort();

    // GraphML interface
    

    void setAggregate(Aggregate *aggregate);
    Aggregate* getAggregate();
    void unsetAggregate();

    bool canAdoptChild(Node *node);
    Edge::EDGE_CLASS canConnect(Node *node);

private:
    Aggregate* aggregate;
};

#endif // EVENTPORT_H
