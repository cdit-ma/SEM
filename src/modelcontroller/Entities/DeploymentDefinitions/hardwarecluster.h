#ifndef HARDWARECLUSTER_H
#define HARDWARECLUSTER_H
#include "../node.h"

class EntityFactoryRegistryBroker;
class HardwareCluster : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    HardwareCluster(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // HARDWARECLUSTER_H
