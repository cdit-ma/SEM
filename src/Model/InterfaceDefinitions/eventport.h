#ifndef EVENTPORT_H
#define EVENTPORT_H
#include "../node.h"

class Aggregate;

class EntityFactory;
class EventPort : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    EventPort(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    EventPort(NODE_KIND kind);
public:
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
