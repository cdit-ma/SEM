#ifndef MEMBER_H
#define MEMBER_H
#include "../node.h"

class EntityFactory;
class Member : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Member(EntityFactory* factory);
	Member();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // MEMBER_H
