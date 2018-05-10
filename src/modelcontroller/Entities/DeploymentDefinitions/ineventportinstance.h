#ifndef INEVENTPORTINSTANCE_H
#define INEVENTPORTINSTANCE_H
#include "eventportassembly.h"


class EntityFactoryRegistryBroker;
class InEventPortInstance : public EventPortAssembly
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    InEventPortInstance(EntityFactoryBroker& factory, bool is_temp_node);
private:
	void updateQOSEdge();
};

#endif // INEVENTPORTINSTANCE_H
