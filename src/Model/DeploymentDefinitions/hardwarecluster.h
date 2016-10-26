#ifndef HARDWARECLUSTER_H
#define HARDWARECLUSTER_H
#include "../node.h"
class HardwareCluster : public Node
{
    Q_OBJECT
public:
    HardwareCluster();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // HARDWARECLUSTER_H
