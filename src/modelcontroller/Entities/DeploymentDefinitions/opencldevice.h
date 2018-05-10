#ifndef OPENCLDEVICE_H
#define OPENCLDEVICE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class OpenCLDevice : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    OpenCLDevice(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // OPENCLDEVICE_H
