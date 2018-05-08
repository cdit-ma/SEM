#ifndef OPENCLDEVICE_H
#define OPENCLDEVICE_H
#include "../node.h"

class EntityFactory;
class OpenCLDevice : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    OpenCLDevice(EntityFactory& factory, bool is_temp_node);
};

#endif // OPENCLDEVICE_H
