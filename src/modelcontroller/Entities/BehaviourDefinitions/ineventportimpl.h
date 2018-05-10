#ifndef INEVENTPORTIMPL_H
#define INEVENTPORTIMPL_H
#include "containernode.h"


class EntityFactoryRegistryBroker;
class InEventPortImpl : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	InEventPortImpl(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};


#endif // INEVENTPORT_H
