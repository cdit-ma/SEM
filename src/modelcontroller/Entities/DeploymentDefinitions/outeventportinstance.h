#ifndef OUTEVENTPORTINSTANCE_H
#define OUTEVENTPORTINSTANCE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class OutEventPortInstance : public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    OutEventPortInstance(EntityFactoryBroker& factory, bool is_temp_node);
private:
	void updateQOSEdge();
};

#endif // OUTEVENTPORTINSTANCE_H
