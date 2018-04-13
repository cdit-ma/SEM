#ifndef BRANCH_H
#define BRANCH_H
#include "../node.h"

class EntityFactory;
class Branch: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Branch(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    Branch(NODE_KIND kind);
public:
    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // BRANCH_H
