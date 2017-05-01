#ifndef BLACKBOX_H
#define BLACKBOX_H
#include "../node.h"

class EntityFactory;
class BlackBox : public Node
{
    Q_OBJECT
protected:
	BlackBox(EntityFactory* factory);
	BlackBox();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // BLACKBOX_H
