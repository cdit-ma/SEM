#ifndef HARDWARECLUSTER_H
#define HARDWARECLUSTER_H
#include "../node.h"
class EntityFactory;
class HardwareCluster : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	HardwareCluster(EntityFactory* factory);
	HardwareCluster();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // HARDWARECLUSTER_H
