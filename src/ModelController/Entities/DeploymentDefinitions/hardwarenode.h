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
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // HARDWARENODE_H
