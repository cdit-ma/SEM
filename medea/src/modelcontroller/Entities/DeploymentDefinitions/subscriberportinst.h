#ifndef PORT_SUBSCRIBER_INST_H
#define PORT_SUBSCRIBER_INST_H
#include "eventportassembly.h"

class EntityFactoryRegistryBroker;
class SubscriberPortInst : public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    SubscriberPortInst(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // PORT_SUBSCRIBER_INST_H
