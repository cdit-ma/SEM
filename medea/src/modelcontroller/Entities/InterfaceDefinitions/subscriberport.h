
#ifndef PORT_SUBSCRIBER_H
#define PORT_SUBSCRIBER_H
#include "eventport.h"


class EntityFactoryRegistryBroker;
class SubscriberPort : public EventPort
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    SubscriberPort(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // PORT_SUBSCRIBER_H
