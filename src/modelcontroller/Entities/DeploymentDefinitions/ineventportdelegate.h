#ifndef INEVENTPORTDELEGATE_H
#define INEVENTPORTDELEGATE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class InEventPortDelegate: public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    InEventPortDelegate(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // INEVENTPORTDELEGATE_H
