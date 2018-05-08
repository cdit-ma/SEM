#ifndef HARDWARECLUSTER_H
#define HARDWARECLUSTER_H
#include "../node.h"
class EntityFactory;
class HardwareCluster : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    HardwareCluster(EntityFactory& factory, bool is_temp_node);
};

#endif // HARDWARECLUSTER_H
