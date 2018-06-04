#ifndef PORT_SUBSCRIBER_DELEGATE_H
#define PORT_SUBSCRIBER_DELEGATE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class SubscriberPortDelegate: public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    SubscriberPortDelegate(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // PORT_SUBSCRIBER_DELEGATE_H
