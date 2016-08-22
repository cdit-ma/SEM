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

    bool canAdoptChild(Node *node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);

private:
    Aggregate* aggregate;
};

#endif // EVENTPORT_H
