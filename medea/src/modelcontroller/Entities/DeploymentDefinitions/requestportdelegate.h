#ifndef PORT_REQUESTER_DELEGATE_H
#define PORT_REQUESTER_DELEGATE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class RequestPortDelegate: public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    RequestPortDelegate(EntityFactoryBroker& factory, bool is_temp_node);
};


#endif // PORT_REQUESTER_DELEGATE_H
