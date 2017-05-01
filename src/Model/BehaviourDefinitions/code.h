#ifndef MODEL_IMPL_CODE_H
#define MODEL_IMPL_CODE_H

#include "behaviournode.h"

class EntityFactory;
class Code: public BehaviourNode
{
    Q_OBJECT
protected:
	Code(EntityFactory* factory);
	Code();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // MODEL_IMPL_CODE_H
