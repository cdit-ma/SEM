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
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // OPENCLDEVICE_H
