#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class EntityFactory;
class HardwareNode : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	HardwareNode(EntityFactory* factory);
	HardwareNode();
};

#endif // HARDWARENODE_H
