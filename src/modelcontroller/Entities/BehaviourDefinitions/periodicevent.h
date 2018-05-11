#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "containernode.h"

class EntityFactoryRegistryBroker;
class PeriodicEvent: public Node, public ContainerNode
{
    Q_OBJECT
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	PeriodicEvent(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};

#endif // PERIODICEVENT_H

