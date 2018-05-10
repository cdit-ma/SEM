#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class HardwareNode : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    HardwareNode(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // HARDWARENODE_H
