#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "containernode.h"

class EntityFactory;
class InEventPortImpl : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	InEventPortImpl(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};


#endif // INEVENTPORT_H
