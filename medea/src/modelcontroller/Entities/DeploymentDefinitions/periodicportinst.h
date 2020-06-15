#ifndef PORT_PERIODIC_INST_H
#define PORT_PERIODIC_INST_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class PeriodicPortInst : public Node
{
    Q_OBJECT
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	PeriodicPortInst(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};

#endif // PORT_PERIODIC_INST_H

