#ifndef PORT_PUBLISHER_DELEGATE_H
#define PORT_PUBLISHER_DELEGATE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class PublisherPortDelegate: public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    PublisherPortDelegate(EntityFactoryBroker& factory, bool is_temp_node);
};


#endif // PORT_PUBLISHER_DELEGATE_H
