#ifndef PORT_PUBLISHER_INST_H
#define PORT_PUBLISHER_INST_H
#include "eventportassembly.h"

class EntityFactoryRegistryBroker;
class PublisherPortInst : public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    PublisherPortInst(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // PORT_PUBLISHER_INST_H
