#ifndef OPENCLDEVICE_H
#define OPENCLDEVICE_H
#include "../node.h"

class EntityFactory;
class OpenCLDevice : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	OpenCLDevice(EntityFactory* factory);
	OpenCLDevice();
};

#endif // OPENCLDEVICE_H
