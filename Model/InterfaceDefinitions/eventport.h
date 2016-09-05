#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class Aggregate;

class EventPort : public Node
{
    Q_OBJECT
public:
    EventPort(Node::NODE_KIND kind);
    bool isInPort() const;
    bool isOutPort() const;

    void setAggregate(Aggregate *aggregate);
    Aggregate* getAggregate();
    void unsetAggregate();

    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

private:
    Aggregate* aggregate;
};

#endif // EVENTPORT_H
