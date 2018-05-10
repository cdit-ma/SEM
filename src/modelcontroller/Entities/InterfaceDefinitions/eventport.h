#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class Aggregate;

class EventPort : public Node
{
    Q_OBJECT

protected:
    EventPort(EntityFactoryBroker& factory, NODE_KIND kind, bool is_temp);
public:
    bool isInPort() const;
    bool isOutPort() const;

    void setAggregate(Aggregate *aggregate);
    Aggregate* getAggregate();
    void unsetAggregate();

    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

private:
    Aggregate* aggregate = 0;
};

#endif // EVENTPORT_H
