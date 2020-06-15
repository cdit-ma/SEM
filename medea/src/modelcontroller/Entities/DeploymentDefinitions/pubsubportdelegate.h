#ifndef PORT_PUBSUB_DELEGATE_H
#define PORT_PUBSUB_DELEGATE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class PubSubPortDelegate: public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    PubSubPortDelegate(EntityFactoryBroker& factory, bool is_temp_node);
};


#endif // PORT_PUBSUB_DELEGATE_H
