#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportdelegate.h"

class EntityFactory;
class OutEventPortDelegate: public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortDelegate(EntityFactory* factory);
	OutEventPortDelegate();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // OUTEVENTPORTDELEGATE_H
