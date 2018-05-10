#ifndef INEVENTPORT_H
#define INEVENTPORT_H
#include "eventport.h"


class EntityFactoryRegistryBroker;
class InEventPort : public EventPort
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    InEventPort(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // INEVENTPORT_H
