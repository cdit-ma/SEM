#ifndef OUTEVENTPORT_H
#define OUTEVENTPORT_H
#include "eventport.h"


class EntityFactoryRegistryBroker;
class OutEventPort : public EventPort
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    OutEventPort(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // OUTEVENTPORT_H
