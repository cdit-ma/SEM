#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportdelegate.h"

class EntityFactory;
class OutEventPortInstance : public EventPortAssembly
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OutEventPortInstance(EntityFactory* factory);
	OutEventPortInstance();
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // OUTEVENTPORTINSTANCE_H
