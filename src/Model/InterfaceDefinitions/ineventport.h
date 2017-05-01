#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "eventport.h"

class EntityFactory;
class InEventPort : public EventPort
{
    Q_OBJECT
protected:
	InEventPort(EntityFactory* factory);
	InEventPort();
public:
    bool canAdoptChild(Node *node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // INEVENTPORT_H
