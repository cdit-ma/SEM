#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class EntityFactory;
class HardwareNode : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    HardwareNode(EntityFactory& factory, bool is_temp_node);
};

#endif // HARDWARENODE_H
