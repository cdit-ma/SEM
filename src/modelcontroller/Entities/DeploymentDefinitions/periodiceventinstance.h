#ifndef PERIODICEVENT_INSTANCE_H
#define PERIODICEVENT_INSTANCE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class PeriodicEventInstance : public Node
{
    Q_OBJECT
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	PeriodicEventInstance(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};

#endif // PERIODICEVENT_INSTANCE_H

