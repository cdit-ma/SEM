#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "datanode.h"

class EntityFactory;
class MemberInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	MemberInstance(EntityFactory* factory);
	MemberInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MEMBERINSTANCE_H
