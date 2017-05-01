#ifndef WHILELOOP_H
#define WHILELOOP_H
#include "branch.h"

class EntityFactory;
class WhileLoop: public Branch
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	WhileLoop(EntityFactory* factory);
	WhileLoop();
public:
    bool canAdoptChild(Node *node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // WHILELOOP_H
