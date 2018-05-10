#ifndef OUTEVENTPORTDELEGATE_H
#define OUTEVENTPORTDELEGATE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class OutEventPortDelegate: public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    OutEventPortDelegate(EntityFactoryBroker& factory, bool is_temp_node);
};


#endif // OUTEVENTPORTDELEGATE_H
