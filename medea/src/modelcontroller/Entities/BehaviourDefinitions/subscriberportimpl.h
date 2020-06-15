#ifndef PORT_SUBSCRIBER_PORT_IMPL_H
#define PORT_SUBSCRIBER_PORT_IMPL_H
#include "containernode.h"


class EntityFactoryRegistryBroker;
class SubscriberPortImpl : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	SubscriberPortImpl(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};


#endif // PORT_SUBSCRIBER_PORT_IMPL_H
