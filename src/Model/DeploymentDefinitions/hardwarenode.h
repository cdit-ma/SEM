#ifndef HARDWARENODE_H
#define HARDWARENODE_H
#include "../node.h"

class EntityFactory;
class HardwareNode : public Node
{
    Q_OBJECT
protected:
	HardwareNode(EntityFactory* factory);
	HardwareNode();
public:
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // HARDWARENODE_H
