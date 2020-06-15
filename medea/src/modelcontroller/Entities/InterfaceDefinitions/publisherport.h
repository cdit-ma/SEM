#ifndef PORT_PUBLISHER_H
#define PORT_PUBLISHER_H
#include "eventport.h"


class EntityFactoryRegistryBroker;
class PublisherPort : public EventPort
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    PublisherPort(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // PORT_PUBLISHER_H
