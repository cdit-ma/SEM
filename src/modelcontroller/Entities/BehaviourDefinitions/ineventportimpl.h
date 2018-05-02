#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "containernode.h"

class EntityFactory;
class InEventPortImpl : public Node, public ContainerNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	InEventPortImpl(EntityFactory* factory);
	InEventPortImpl();
public:
    bool canAdoptChild(Node* child);
};


#endif // INEVENTPORT_H
